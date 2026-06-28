#include "VulkanSoftwareVoxelRasterizer.h"

#include "BlockDefinitions.h"
#include "Camera.h"
#include "Logger.h"
#include "VulkanSoftwareRasterizerColor.h"
#include "WorkerPolicy.h"
#include "World.h"

#include <boost/container/flat_map.hpp>
#include <stb_image.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <limits>
#include <memory>
#include <string>

namespace ve::rendering
{
	namespace
	{
		struct RayHit
		{
			bool hit = false;
			ve::blocks::BlockId block = ve::blocks::BlockId::Air;
			glm::ivec3 position{ 0 };
			glm::ivec3 normal{ 0, 1, 0 };
			float distance = 0.0f;
		};

		constexpr int kMaxRaySteps = 256;

		bool IsSolid(ve::blocks::BlockId block) noexcept
		{
			const auto index = static_cast<std::size_t>(block);
			return index < static_cast<std::size_t>(ve::blocks::BlockId::Count) &&
				ve::blocks::BuiltInBlockDefinitions[index].isSolid;
		}



		ve::blocks::BlockFace FaceForNormal(const glm::ivec3& normal) noexcept
		{
			if (normal.y > 0) return ve::blocks::BlockFace::Top;
			if (normal.y < 0) return ve::blocks::BlockFace::Bottom;
			if (normal.z > 0) return ve::blocks::BlockFace::Front;
			if (normal.z < 0) return ve::blocks::BlockFace::Back;
			if (normal.x > 0) return ve::blocks::BlockFace::Right;
			return ve::blocks::BlockFace::Left;
		}

		glm::vec2 HitUv(const RayHit& hit, const glm::vec3& origin, const glm::vec3& direction) noexcept
		{
			const glm::vec3 point = origin + (direction * std::max(hit.distance, 0.0f));
			auto fract = [](float value)
			{
				const float f = value - std::floor(value);
				return std::clamp(f, 0.0f, 0.9999f);
			};
			if (hit.normal.y != 0) return { fract(point.x), fract(point.z) };
			if (hit.normal.x != 0) return { fract(point.z), fract(point.y) };
			return { fract(point.x), fract(point.y) };
		}

		int FloorToInt(float value) noexcept
		{
			return static_cast<int>(std::floor(value));
		}

		float InitialAxisDistance(float origin, float direction) noexcept
		{
			if (direction > 0.0f) return (std::floor(origin) + 1.0f - origin) / direction;
			if (direction < 0.0f) return (origin - std::floor(origin)) / -direction;
			return std::numeric_limits<float>::infinity();
		}

		float AxisDelta(float direction) noexcept
		{
			return direction == 0.0f ? std::numeric_limits<float>::infinity() : std::abs(1.0f / direction);
		}

		glm::vec3 FindAirCameraOrigin(const VulkanSoftwareVoxelRasterizer::FrameWorldSnapshot& world, glm::vec3 origin)
		{
			for (int attempts = 0; attempts < 32; ++attempts)
			{
				const auto block = world.GetBlock({ FloorToInt(origin.x), FloorToInt(origin.y), FloorToInt(origin.z) });
				if (!IsSolid(block)) return origin;
				origin.y += 1.0f;
			}
			return origin;
		}

		RayHit TraceWorld(const VulkanSoftwareVoxelRasterizer::FrameWorldSnapshot& world, const glm::vec3& origin, const glm::vec3& direction, float max_ray_distance)
		{
			glm::ivec3 block{ FloorToInt(origin.x), FloorToInt(origin.y), FloorToInt(origin.z) };
			const glm::ivec3 step{
				direction.x > 0.0f ? 1 : (direction.x < 0.0f ? -1 : 0),
				direction.y > 0.0f ? 1 : (direction.y < 0.0f ? -1 : 0),
				direction.z > 0.0f ? 1 : (direction.z < 0.0f ? -1 : 0)
			};
			glm::vec3 max_distance{
				InitialAxisDistance(origin.x, direction.x),
				InitialAxisDistance(origin.y, direction.y),
				InitialAxisDistance(origin.z, direction.z)
			};
			const glm::vec3 delta{
				AxisDelta(direction.x),
				AxisDelta(direction.y),
				AxisDelta(direction.z)
			};
			glm::ivec3 normal{ 0, 1, 0 };
			float distance = 0.0f;

			for (int ray_step = 0; ray_step < kMaxRaySteps && distance <= max_ray_distance; ++ray_step)
			{
				const bool inside_world = block.x >= 0 && block.y >= 0 && block.z >= 0 &&
					block.x < world.width && block.y < world.height && block.z < world.depth;
				if (inside_world)
				{
					const std::size_t block_index =
						((static_cast<std::size_t>(block.x) * static_cast<std::size_t>(world.height)) + static_cast<std::size_t>(block.y)) *
						static_cast<std::size_t>(world.depth) + static_cast<std::size_t>(block.z);
					const ve::blocks::BlockId block_id = world.blocks[block_index];
					if (IsSolid(block_id)) return { true, block_id, block, normal, distance };
				}
				else if ((block.x < 0 && step.x <= 0) || (block.x >= world.width && step.x >= 0) ||
					(block.y < 0 && step.y <= 0) || (block.y >= world.height && step.y >= 0) ||
					(block.z < 0 && step.z <= 0) || (block.z >= world.depth && step.z >= 0))
				{
					return {};
				}

				if (max_distance.x < max_distance.y && max_distance.x < max_distance.z)
				{
					block.x += step.x;
					distance = max_distance.x;
					max_distance.x += delta.x;
					normal = { -step.x, 0, 0 };
				}
				else if (max_distance.y < max_distance.z)
				{
					block.y += step.y;
					distance = max_distance.y;
					max_distance.y += delta.y;
					normal = { 0, -step.y, 0 };
				}
				else
				{
					block.z += step.z;
					distance = max_distance.z;
					max_distance.z += delta.z;
					normal = { 0, 0, -step.z };
				}
			}
			return {};
		}

		Rgb ShadeHit(const RayHit& hit, const glm::vec3& direction, Rgb base_color, float max_ray_distance, float fog_strength) noexcept
		{
			float shade = 0.72f;
			if (hit.normal.y > 0) shade = 1.05f;
			else if (hit.normal.y < 0) shade = 0.45f;
			else if (hit.normal.x != 0) shade = 0.78f;

			Rgb color = base_color;
			color = { Scale(color.r, shade), Scale(color.g, shade), Scale(color.b, shade) };
			const float fog = std::clamp(hit.distance / std::max(max_ray_distance, 1.0f), 0.0f, 1.0f);
			return Mix(color, SkyColor(direction.y), fog * std::clamp(fog_strength, 0.0f, 1.0f));
		}

		std::uint32_t SampleStepFor(const VulkanDemoSettings& settings) noexcept
		{
			return std::clamp(settings.pixel_block_size, 1u, 8u);
		}

		std::uint32_t EffectiveSampleStepFor(const VulkanDemoSettings& settings, const VulkanFrameTiming& previous_timing) noexcept
		{
			const std::uint32_t requested = SampleStepFor(settings);
			if (!settings.adaptive_quality || previous_timing.render_extent.width == 0u || previous_timing.render_extent.height == 0u)
			{
				return requested;
			}

			const double previous_cpu_ms =
				previous_timing.raster_cpu_ms +
				previous_timing.upscale_cpu_ms +
				previous_timing.upload_cpu_ms +
				previous_timing.present_cpu_ms;
			if (previous_cpu_ms > 80.0) return std::max(requested, 8u);
			if (previous_cpu_ms > 45.0) return std::max(requested, 6u);
			if (previous_cpu_ms > 28.0) return std::max(requested, 4u);
			if (previous_cpu_ms > 18.0) return std::max(requested, 3u);
			return requested;
		}

		VkExtent2D InternalRenderExtentFor(VkExtent2D output_extent, const VulkanDemoSettings& settings) noexcept
		{
			const std::uint32_t max_width = std::clamp(settings.max_internal_width, 320u, 1920u);
			const std::uint32_t max_height = std::clamp(settings.max_internal_height, 180u, 1080u);
			if (output_extent.width <= max_width && output_extent.height <= max_height)
			{
				return output_extent;
			}
			const std::uint64_t width_limited_height = static_cast<std::uint64_t>(output_extent.height) * max_width;
			const std::uint64_t height_limited_width = static_cast<std::uint64_t>(output_extent.width) * max_height;
			if (width_limited_height <= height_limited_width)
			{
				return {
					max_width,
					std::max(1u, static_cast<std::uint32_t>(width_limited_height / output_extent.width))
				};
			}
			return {
				std::max(1u, static_cast<std::uint32_t>(height_limited_width / output_extent.height)),
				max_height
			};
		}

		bool NearlyEqual(const glm::vec3& left, const glm::vec3& right) noexcept
		{
			constexpr float kEpsilon = 0.0001f;
			return glm::all(glm::lessThan(glm::abs(left - right), glm::vec3{ kEpsilon }));
		}


	}

	VulkanSoftwareVoxelRasterizer::~VulkanSoftwareVoxelRasterizer()
	{
		Release();
	}

	ve::blocks::BlockId VulkanSoftwareVoxelRasterizer::FrameWorldSnapshot::GetBlock(const glm::ivec3& position) const noexcept
	{
		if (position.x < 0 || position.y < 0 || position.z < 0 ||
			position.x >= width || position.y >= height || position.z >= depth)
		{
			return ve::blocks::BlockId::Air;
		}
		const std::size_t index =
			((static_cast<std::size_t>(position.x) * static_cast<std::size_t>(height)) + static_cast<std::size_t>(position.y)) *
			static_cast<std::size_t>(depth) + static_cast<std::size_t>(position.z);
		return blocks[index];
	}

	std::uint32_t VulkanSoftwareVoxelRasterizer::CpuTexture::Sample(float u, float v) const noexcept
	{
		if (pixels.empty()) return PackRgb({ 132, 132, 132 });
		const float wrapped_u = u - std::floor(u);
		const float wrapped_v = v - std::floor(v);
		const auto x = static_cast<std::uint32_t>(std::clamp(wrapped_u, 0.0f, 0.9999f) * static_cast<float>(width));
		const auto y = static_cast<std::uint32_t>(std::clamp(1.0f - wrapped_v, 0.0f, 0.9999f) * static_cast<float>(height));
		const std::size_t index = static_cast<std::size_t>(std::min(y, height - 1u)) * width + std::min(x, width - 1u);
		return pixels[index];
	}

	bool VulkanSoftwareVoxelRasterizer::Resize(VkExtent2D extent, const VulkanDemoSettings& settings)
	{
		if (extent.width == 0 || extent.height == 0) return false;
		const VkExtent2D next_render_extent = InternalRenderExtentFor(extent, settings);
		if (extent_.width == extent.width && extent_.height == extent.height &&
			render_extent_.width == next_render_extent.width && render_extent_.height == next_render_extent.height)
		{
			return true;
		}
		extent_ = extent;
		render_extent_ = next_render_extent;
		pixels_.assign(static_cast<std::size_t>(extent.width) * static_cast<std::size_t>(extent.height), 0u);
		render_pixels_.assign(static_cast<std::size_t>(render_extent_.width) * static_cast<std::size_t>(render_extent_.height), 0u);
		RebuildUpscaleLookup();
		ray_cache_valid_ = false;
		return true;
	}

	void VulkanSoftwareVoxelRasterizer::Release()
	{
		{
			std::lock_guard lock(work_mutex_);
			stop_workers_ = true;
			++work_generation_;
		}
		work_available_.notify_all();
		workers_.clear();
		{
			std::lock_guard lock(work_mutex_);
			stop_workers_ = false;
			active_workers_ = 0;
			current_work_ = {};
		}
		pixels_.clear();
		render_pixels_.clear();
		outline_pixels_.clear();
		upscale_x_ranges_.clear();
		upscale_y_ranges_.clear();
		ray_cache_.clear();
		world_snapshot_ = {};
		extent_ = {};
		render_extent_ = {};
		ray_cache_valid_ = false;
		last_timing_ = {};
	}

	void VulkanSoftwareVoxelRasterizer::CaptureWorldSnapshot(const ve::world::World& world)
	{
		const ve::world::WorldMetrics metrics = world.Metrics();
		world_snapshot_.width = metrics.worldSizeChunks * ve::world::terrain::ChunkWidth;
		world_snapshot_.height = ve::world::terrain::ChunkHeight;
		world_snapshot_.depth = metrics.worldSizeChunks * ve::world::terrain::ChunkDepth;
		const std::size_t block_count =
			static_cast<std::size_t>(world_snapshot_.width) *
			static_cast<std::size_t>(world_snapshot_.height) *
			static_cast<std::size_t>(world_snapshot_.depth);
		world_snapshot_.blocks.resize(block_count);
		std::size_t index = 0;
		for (int x = 0; x < world_snapshot_.width; ++x)
		{
			for (int y = 0; y < world_snapshot_.height; ++y)
			{
				for (int z = 0; z < world_snapshot_.depth; ++z)
				{
					world_snapshot_.blocks[index++] = world.GetBlock(x, y, z);
				}
			}
		}
		world_snapshot_.world_revision = world.Revision();
	}

	void VulkanSoftwareVoxelRasterizer::EnsureRayCache(const Camera& camera, std::uint32_t sample_step)
	{
		const glm::vec3 forward = glm::normalize(camera.GetForward());
		const glm::vec3 right = glm::normalize(camera.GetRight());
		const glm::vec3 up = glm::normalize(camera.GetUp());
		if (ray_cache_valid_ &&
			cached_sample_step_ == sample_step &&
			NearlyEqual(cached_forward_, forward) &&
			NearlyEqual(cached_right_, right) &&
			NearlyEqual(cached_up_, up))
		{
			return;
		}

		const float width = static_cast<float>(render_extent_.width);
		const float height = static_cast<float>(render_extent_.height);
		const float aspect = width / height;
		const float tan_half_fov = std::tan(glm::radians(35.0f));
		ray_cache_.clear();
		const std::size_t sample_columns = (render_extent_.width + sample_step - 1u) / sample_step;
		const std::size_t sample_rows = (render_extent_.height + sample_step - 1u) / sample_step;
		ray_cache_.reserve(sample_columns * sample_rows);

		for (std::uint32_t y = 0; y < render_extent_.height; y += sample_step)
		{
			for (std::uint32_t x = 0; x < render_extent_.width; x += sample_step)
			{
				const float screen_x = ((static_cast<float>(x) + 0.5f) / width) * 2.0f - 1.0f;
				const float screen_y = 1.0f - ((static_cast<float>(y) + 0.5f) / height) * 2.0f;
				ray_cache_.push_back({
					x,
					y,
					std::min(x + sample_step, render_extent_.width),
					std::min(y + sample_step, render_extent_.height),
					glm::normalize(forward + (right * screen_x * tan_half_fov * aspect) + (up * screen_y * tan_half_fov))
				});
			}
		}

		cached_forward_ = forward;
		cached_right_ = right;
		cached_up_ = up;
		cached_sample_step_ = sample_step;
		ray_cache_valid_ = true;
	}

	void VulkanSoftwareVoxelRasterizer::RenderSamples(const RasterWork& work, std::size_t begin, std::size_t end)
	{
		for (std::size_t index = begin; index < end; ++index)
		{
			const CachedSampleRay& sample = ray_cache_[index];
			const RayHit hit = TraceWorld(*work.world, work.origin, sample.direction, work.max_ray_distance);
			Rgb base_color = SkyColor(sample.direction.y);
			if (hit.hit)
			{
				const ve::blocks::BlockFace face = FaceForNormal(hit.normal);
				const glm::vec2 uv = HitUv(hit, work.origin, sample.direction);
				base_color = ApplyBlockTint(hit.block, face, UnpackRgb(TextureFor(hit.block, face).Sample(uv.x, uv.y)));
			}
			const Rgb color = hit.hit ? ShadeHit(hit, sample.direction, base_color, work.max_ray_distance, work.fog_strength) : base_color;
			const std::uint32_t packed = PackColor(color, work.format);
			if (sample.x_end == sample.x + 1u && sample.y_end == sample.y + 1u)
			{
				render_pixels_[(static_cast<std::size_t>(sample.y) * render_extent_.width) + sample.x] = packed;
				continue;
			}
			for (std::uint32_t fill_y = sample.y; fill_y < sample.y_end; ++fill_y)
			{
				std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(fill_y) * render_extent_.width);
				std::fill(row + sample.x, row + sample.x_end, packed);
			}
		}
	}

	void VulkanSoftwareVoxelRasterizer::Render(const VulkanSoftwareVoxelRasterizerFrame& frame)
	{
		if (frame.input.toggle_debug_overlay) frame.settings.show_debug_overlay = !frame.settings.show_debug_overlay;
		if (frame.input.toggle_tuning_panel) frame.settings.show_tuning_panel = !frame.settings.show_tuning_panel;
		if (!Resize(frame.extent, frame.settings)) return;

		const std::uint64_t world_revision = frame.world.Revision();
		const auto snapshot_start = std::chrono::steady_clock::now();
		if (world_snapshot_.blocks.empty() || world_snapshot_.world_revision != world_revision)
		{
			CaptureWorldSnapshot(frame.world);
		}
		const auto snapshot_end = std::chrono::steady_clock::now();
		const glm::vec3 origin = FindAirCameraOrigin(world_snapshot_, frame.camera.GetPosition());
		const std::uint32_t sample_step = EffectiveSampleStepFor(frame.settings, frame.previous_timing);
		EnsureRayCache(frame.camera, sample_step);

		const RasterWork work{ &world_snapshot_, origin, frame.format, frame.settings.max_ray_distance, frame.settings.fog_strength };
		const auto raster_start = std::chrono::steady_clock::now();
		RenderSamplesMultithreaded(work);
		const auto raster_end = std::chrono::steady_clock::now();
		ApplyVoxelOutlines(frame.format, frame.settings.outline_strength);
		if (frame.settings.show_debug_overlay) DrawDemoOverlay(frame);
		if (frame.settings.show_tuning_panel) DrawTuningPanel(frame);
		DrawCrosshair(frame.format);
		const auto overlay_end = std::chrono::steady_clock::now();

		last_timing_.snapshot_cpu_ms = std::chrono::duration<double, std::milli>(snapshot_end - snapshot_start).count();
		last_timing_.raster_cpu_ms = std::chrono::duration<double, std::milli>(raster_end - raster_start).count();
		last_timing_.upscale_cpu_ms = std::chrono::duration<double, std::milli>(overlay_end - raster_end).count();
		last_timing_.worker_count = static_cast<std::uint32_t>(workers_.size() + 1u);
		last_timing_.sample_step = sample_step;
		last_timing_.render_extent = render_extent_;
	}

	std::span<const std::uint32_t> VulkanSoftwareVoxelRasterizer::Pixels() const noexcept
	{
		return pixels_;
	}

	VkExtent2D VulkanSoftwareVoxelRasterizer::Extent() const noexcept
	{
		return extent_;
	}

	std::span<const std::uint32_t> VulkanSoftwareVoxelRasterizer::RenderPixels() const noexcept
	{
		return render_pixels_;
	}

	VkExtent2D VulkanSoftwareVoxelRasterizer::RenderExtent() const noexcept
	{
		return render_extent_;
	}

	const VulkanFrameTiming& VulkanSoftwareVoxelRasterizer::LastTiming() const noexcept
	{
		return last_timing_;
	}


}
