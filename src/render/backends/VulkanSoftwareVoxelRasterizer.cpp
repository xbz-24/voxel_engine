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

		std::size_t FaceIndex(ve::blocks::BlockId block, ve::blocks::BlockFace face) noexcept
		{
			return (static_cast<std::size_t>(block) * static_cast<std::size_t>(ve::blocks::BlockFace::Count)) +
				static_cast<std::size_t>(face);
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

		constexpr std::size_t kSamplesPerBatch = 512u;

		std::uint32_t CeilDiv(std::uint64_t numerator, std::uint32_t denominator) noexcept
		{
			return static_cast<std::uint32_t>((numerator + static_cast<std::uint64_t>(denominator) - 1ull) / denominator);
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

		std::uint8_t DarkenChannel(std::uint8_t value, float strength) noexcept
		{
			const float scale = 1.0f - std::clamp(strength, 0.0f, 0.85f);
			return static_cast<std::uint8_t>(std::clamp(static_cast<float>(value) * scale, 0.0f, 255.0f));
		}

		std::uint32_t DarkenPacked(std::uint32_t packed, VkFormat format, float strength) noexcept
		{
			const bool bgra = IsBgraFormat(format);
			const std::uint8_t c0 = static_cast<std::uint8_t>(packed & 0xffu);
			const std::uint8_t c1 = static_cast<std::uint8_t>((packed >> 8u) & 0xffu);
			const std::uint8_t c2 = static_cast<std::uint8_t>((packed >> 16u) & 0xffu);
			const Rgb color = bgra ? Rgb{ c2, c1, c0 } : Rgb{ c0, c1, c2 };
			return PackColor({ DarkenChannel(color.r, strength), DarkenChannel(color.g, strength), DarkenChannel(color.b, strength) }, format);
		}

		int ColorDelta(std::uint32_t left, std::uint32_t right) noexcept
		{
			const int b0 = static_cast<int>(left & 0xffu) - static_cast<int>(right & 0xffu);
			const int b1 = static_cast<int>((left >> 8u) & 0xffu) - static_cast<int>((right >> 8u) & 0xffu);
			const int b2 = static_cast<int>((left >> 16u) & 0xffu) - static_cast<int>((right >> 16u) & 0xffu);
			return std::abs(b0) + std::abs(b1) + std::abs(b2);
		}

		bool NearlyEqual(const glm::vec3& left, const glm::vec3& right) noexcept
		{
			constexpr float kEpsilon = 0.0001f;
			return glm::all(glm::lessThan(glm::abs(left - right), glm::vec3{ kEpsilon }));
		}

		std::array<std::uint8_t, 7> GlyphFor(char value) noexcept
		{
			switch (value)
			{
			case '0': return { { 0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110 } };
			case '1': return { { 0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110 } };
			case '2': return { { 0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111 } };
			case '3': return { { 0b11110, 0b00001, 0b00001, 0b01110, 0b00001, 0b00001, 0b11110 } };
			case '4': return { { 0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010 } };
			case '5': return { { 0b11111, 0b10000, 0b10000, 0b11110, 0b00001, 0b00001, 0b11110 } };
			case '6': return { { 0b01110, 0b10000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110 } };
			case '7': return { { 0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000 } };
			case '8': return { { 0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110 } };
			case '9': return { { 0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00001, 0b01110 } };
			case 'A': return { { 0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 } };
			case 'C': return { { 0b01111, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b01111 } };
			case 'D': return { { 0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110 } };
			case 'E': return { { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111 } };
			case 'F': return { { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000 } };
			case 'G': return { { 0b01111, 0b10000, 0b10000, 0b10111, 0b10001, 0b10001, 0b01111 } };
			case 'H': return { { 0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 } };
			case 'I': return { { 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111 } };
			case 'K': return { { 0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001 } };
			case 'L': return { { 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111 } };
			case 'M': return { { 0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001 } };
			case 'N': return { { 0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001 } };
			case 'O': return { { 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 } };
			case 'P': return { { 0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000 } };
			case 'R': return { { 0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001 } };
			case 'S': return { { 0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110 } };
			case 'T': return { { 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100 } };
			case 'U': return { { 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 } };
			case 'V': return { { 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b01010, 0b00100 } };
			case 'X': return { { 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b01010, 0b10001 } };
			case 'Y': return { { 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100 } };
			case '.': return { { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b01100, 0b01100 } };
			case '-': return { { 0b00000, 0b00000, 0b00000, 0b11111, 0b00000, 0b00000, 0b00000 } };
			default: return { { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000 } };
			}
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

	void VulkanSoftwareVoxelRasterizer::LoadBlockTextures(const std::filesystem::path& block_texture_directory)
	{
		texture_library_ = {};
		texture_library_.textures.push_back(CpuTexture{ std::vector<std::uint32_t>{ PackRgb({ 132, 132, 132 }) }, 1u, 1u });

		boost::container::flat_map<std::string, std::uint16_t> texture_indices;
		const auto load_texture = [&](const char* file_name, Rgb fallback) -> std::uint16_t
		{
			if (file_name == nullptr) return 0u;
			const std::string key{ file_name };
			if (const auto existing = texture_indices.find(key); existing != texture_indices.end()) return existing->second;

			CpuTexture texture;
			const std::filesystem::path texture_path = block_texture_directory / key;
			int width = 0;
			int height = 0;
			int channels = 0;
			unsigned char* data = stbi_load(texture_path.string().c_str(), &width, &height, &channels, 4);
			if (data != nullptr && width > 0 && height > 0)
			{
				texture.width = static_cast<std::uint32_t>(width);
				texture.height = static_cast<std::uint32_t>(height);
				texture.pixels.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
				for (std::size_t index = 0; index < texture.pixels.size(); ++index)
				{
					const unsigned char* pixel = data + (index * 4u);
					const Rgb color{ pixel[0], pixel[1], pixel[2] };
					texture.pixels[index] = pixel[3] < 16u ? PackRgb(fallback) : PackRgb(color);
				}
			}
			else
			{
				texture = CpuTexture{ std::vector<std::uint32_t>{ PackRgb(fallback) }, 1u, 1u };
			}
			stbi_image_free(data);

			const auto next_index = static_cast<std::uint16_t>(texture_library_.textures.size());
			texture_library_.textures.push_back(std::move(texture));
			texture_indices.emplace(key, next_index);
			return next_index;
		};

		for (const ve::blocks::BlockDefinition& definition : ve::blocks::BuiltInBlockDefinitions)
		{
			const Rgb fallback = BaseBlockColor(definition.id);
			const std::array<const char*, static_cast<std::size_t>(ve::blocks::BlockFace::Count)> files{
				definition.textures.top,
				definition.textures.bottom,
				definition.textures.front,
				definition.textures.back,
				definition.textures.left,
				definition.textures.right
			};
			for (std::size_t face = 0; face < files.size(); ++face)
			{
				texture_library_.face_texture_indices[FaceIndex(definition.id, static_cast<ve::blocks::BlockFace>(face))] = load_texture(files[face], fallback);
			}
		}
		texture_library_.loaded = true;
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Loaded Vulkan demo block textures: " + std::to_string(texture_library_.textures.size() - 1u));
	}

	const VulkanSoftwareVoxelRasterizer::CpuTexture& VulkanSoftwareVoxelRasterizer::TextureFor(ve::blocks::BlockId block, ve::blocks::BlockFace face) const noexcept
	{
		static const CpuTexture fallback{ std::vector<std::uint32_t>{ PackRgb({ 132, 132, 132 }) }, 1u, 1u };
		if (!texture_library_.loaded || texture_library_.textures.empty()) return fallback;
		const std::size_t block_index = static_cast<std::size_t>(block);
		if (block_index >= static_cast<std::size_t>(ve::blocks::BlockId::Count)) return texture_library_.textures.front();
		const std::uint16_t texture_index = texture_library_.face_texture_indices[FaceIndex(block, face)];
		if (texture_index >= texture_library_.textures.size()) return texture_library_.textures.front();
		return texture_library_.textures[texture_index];
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

	void VulkanSoftwareVoxelRasterizer::EnsureWorkers()
	{
		if (!workers_.empty()) return;
		const std::size_t worker_count = static_cast<std::size_t>(ve::tasks::DefaultWorkerCount(1));
		std::uint64_t initial_generation = 0;
		{
			std::lock_guard lock(work_mutex_);
			initial_generation = work_generation_;
		}
		workers_.reserve(worker_count);
		for (std::size_t index = 0; index < worker_count; ++index)
		{
			workers_.emplace_back([this, initial_generation] { RenderWorkerLoop(initial_generation); });
		}
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

	void VulkanSoftwareVoxelRasterizer::RenderSamplesMultithreaded(const RasterWork& work)
	{
		EnsureWorkers();
		if (workers_.empty() || ray_cache_.size() < 512u)
		{
			RenderSamples(work, 0, ray_cache_.size());
			return;
		}

		{
			std::lock_guard lock(work_mutex_);
			current_work_ = work;
			next_sample_index_.store(0);
			active_workers_ = workers_.size() + 1u;
			++work_generation_;
		}
		work_available_.notify_all();
		for (;;)
		{
			const std::size_t begin = next_sample_index_.fetch_add(kSamplesPerBatch);
			if (begin >= ray_cache_.size()) break;
			RenderSamples(work, begin, std::min(begin + kSamplesPerBatch, ray_cache_.size()));
		}
		CompleteWorkerBatch();

		std::unique_lock lock(work_mutex_);
		work_complete_.wait(lock, [this] { return active_workers_ == 0u; });
	}

	void VulkanSoftwareVoxelRasterizer::RenderWorkerLoop(std::uint64_t initial_generation)
	{
		std::uint64_t observed_generation = initial_generation;
		for (;;)
		{
			RasterWork work{};
			{
				std::unique_lock lock(work_mutex_);
				work_available_.wait(lock, [this, observed_generation] { return stop_workers_ || work_generation_ != observed_generation; });
				if (stop_workers_) return;
				observed_generation = work_generation_;
				work = current_work_;
			}

			for (;;)
			{
				const std::size_t begin = next_sample_index_.fetch_add(kSamplesPerBatch);
				if (begin >= ray_cache_.size()) break;
				RenderSamples(work, begin, std::min(begin + kSamplesPerBatch, ray_cache_.size()));
			}
			CompleteWorkerBatch();
		}
	}

	void VulkanSoftwareVoxelRasterizer::CompleteWorkerBatch()
	{
		std::lock_guard lock(work_mutex_);
		if (active_workers_ > 0u) --active_workers_;
		if (active_workers_ == 0u) work_complete_.notify_one();
	}

	void VulkanSoftwareVoxelRasterizer::RebuildUpscaleLookup()
	{
		upscale_x_ranges_.resize(render_extent_.width);
		for (std::uint32_t x = 0; x < render_extent_.width; ++x)
		{
			upscale_x_ranges_[x] = {
				CeilDiv(static_cast<std::uint64_t>(x) * extent_.width, render_extent_.width),
				CeilDiv(static_cast<std::uint64_t>(x + 1u) * extent_.width, render_extent_.width)
			};
		}

		upscale_y_ranges_.resize(render_extent_.height);
		for (std::uint32_t y = 0; y < render_extent_.height; ++y)
		{
			upscale_y_ranges_[y] = {
				CeilDiv(static_cast<std::uint64_t>(y) * extent_.height, render_extent_.height),
				CeilDiv(static_cast<std::uint64_t>(y + 1u) * extent_.height, render_extent_.height)
			};
		}
	}

	void VulkanSoftwareVoxelRasterizer::UpscaleRenderPixels()
	{
		if (render_extent_.width == extent_.width && render_extent_.height == extent_.height)
		{
			std::copy(render_pixels_.begin(), render_pixels_.end(), pixels_.begin());
			return;
		}
		if (upscale_x_ranges_.size() != render_extent_.width || upscale_y_ranges_.size() != render_extent_.height)
		{
			RebuildUpscaleLookup();
		}
		const std::size_t row_bytes = static_cast<std::size_t>(extent_.width) * sizeof(std::uint32_t);
		for (std::uint32_t source_y = 0; source_y < render_extent_.height; ++source_y)
		{
			const UpscaleRange y_range = upscale_y_ranges_[source_y];
			if (y_range.begin >= y_range.end) continue;

			const std::uint32_t* source_row = render_pixels_.data() + (static_cast<std::size_t>(source_y) * render_extent_.width);
			std::uint32_t* first_destination_row = pixels_.data() + (static_cast<std::size_t>(y_range.begin) * extent_.width);
			for (std::uint32_t source_x = 0; source_x < render_extent_.width; ++source_x)
			{
				const UpscaleRange x_range = upscale_x_ranges_[source_x];
				if (x_range.begin < x_range.end)
				{
					std::fill(first_destination_row + x_range.begin, first_destination_row + x_range.end, source_row[source_x]);
				}
			}

			for (std::uint32_t destination_y = y_range.begin + 1u; destination_y < y_range.end; ++destination_y)
			{
				std::uint32_t* destination_row = pixels_.data() + (static_cast<std::size_t>(destination_y) * extent_.width);
				std::memcpy(destination_row, first_destination_row, row_bytes);
			}
		}
	}

	void VulkanSoftwareVoxelRasterizer::ApplyVoxelOutlines(VkFormat format, float strength)
	{
		if (strength <= 0.01f || render_pixels_.empty() || render_extent_.width < 3u || render_extent_.height < 3u) return;
		outline_pixels_.resize(render_pixels_.size());
		std::copy(render_pixels_.begin(), render_pixels_.end(), outline_pixels_.begin());
		const int threshold = 72;
		for (std::uint32_t y = 1; y + 1u < render_extent_.height; ++y)
		{
			for (std::uint32_t x = 1; x + 1u < render_extent_.width; ++x)
			{
				const std::size_t index = static_cast<std::size_t>(y) * render_extent_.width + x;
				const std::uint32_t center = render_pixels_[index];
				const int dx = ColorDelta(center, render_pixels_[index - 1u]) + ColorDelta(center, render_pixels_[index + 1u]);
				const int dy = ColorDelta(center, render_pixels_[index - render_extent_.width]) + ColorDelta(center, render_pixels_[index + render_extent_.width]);
				if (std::max(dx, dy) > threshold)
				{
					outline_pixels_[index] = DarkenPacked(center, format, strength);
				}
			}
		}
		render_pixels_.swap(outline_pixels_);
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

	void VulkanSoftwareVoxelRasterizer::DrawCrosshair(VkFormat format)
	{
		const std::uint32_t center_x = render_extent_.width / 2u;
		const std::uint32_t center_y = render_extent_.height / 2u;
		const std::uint32_t crosshair = PackColor({ 235, 235, 235 }, format);
		for (std::uint32_t offset = 0; offset < 8u; ++offset)
		{
			if (center_x + offset < render_extent_.width) render_pixels_[(center_y * render_extent_.width) + center_x + offset] = crosshair;
			if (center_x >= offset) render_pixels_[(center_y * render_extent_.width) + center_x - offset] = crosshair;
			if (center_y + offset < render_extent_.height) render_pixels_[((center_y + offset) * render_extent_.width) + center_x] = crosshair;
			if (center_y >= offset) render_pixels_[((center_y - offset) * render_extent_.width) + center_x] = crosshair;
		}
	}

	void VulkanSoftwareVoxelRasterizer::DrawText(const char* text, std::uint32_t x, std::uint32_t y, std::uint32_t scale, std::uint32_t color)
	{
		std::uint32_t cursor_x = x;
		for (const char* cursor = text; *cursor != '\0'; ++cursor)
		{
			const std::array<std::uint8_t, 7> glyph = GlyphFor(*cursor);
			for (std::size_t row_index = 0; row_index < glyph.size(); ++row_index)
			{
				const std::uint8_t row_bits = glyph[row_index];
				for (std::uint32_t column = 0; column < 5u; ++column)
				{
					const std::uint8_t mask = static_cast<std::uint8_t>(1u << (4u - column));
					if ((row_bits & mask) == 0u) continue;

					const std::uint32_t pixel_x = cursor_x + (column * scale);
					const std::uint32_t pixel_y = y + (static_cast<std::uint32_t>(row_index) * scale);
					for (std::uint32_t dy = 0; dy < scale; ++dy)
					{
						if (pixel_y + dy >= render_extent_.height) continue;
						std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(pixel_y + dy) * render_extent_.width);
						for (std::uint32_t dx = 0; dx < scale; ++dx)
						{
							if (pixel_x + dx < render_extent_.width) row[pixel_x + dx] = color;
						}
					}
				}
			}
			cursor_x += 6u * scale;
			if (cursor_x >= render_extent_.width) return;
		}
	}

	void VulkanSoftwareVoxelRasterizer::DrawFilledRect(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height, std::uint32_t color)
	{
		if (x >= render_extent_.width || y >= render_extent_.height || width == 0u || height == 0u) return;
		const std::uint32_t end_x = std::min(x + width, render_extent_.width);
		const std::uint32_t end_y = std::min(y + height, render_extent_.height);
		for (std::uint32_t row_y = y; row_y < end_y; ++row_y)
		{
			std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(row_y) * render_extent_.width);
			std::fill(row + x, row + end_x, color);
		}
	}

	bool VulkanSoftwareVoxelRasterizer::Slider(const VulkanSoftwareVoxelRasterizerFrame& frame,
		const char* label,
		float& value,
		float min_value,
		float max_value,
		std::uint32_t x,
		std::uint32_t y,
		std::uint32_t width)
	{
		const std::uint32_t text = PackColor({ 232, 248, 244 }, frame.format);
		const std::uint32_t rail = PackColor({ 43, 64, 75 }, frame.format);
		const std::uint32_t fill = PackColor({ 87, 220, 180 }, frame.format);
		const std::uint32_t handle = PackColor({ 246, 245, 210 }, frame.format);
		DrawText(label, x, y, 1u, text);
		const std::uint32_t rail_x = x + 104u;
		const std::uint32_t rail_y = y + 2u;
		const std::uint32_t rail_width = std::min(width, render_extent_.width > rail_x ? render_extent_.width - rail_x : 0u);
		if (rail_width == 0u) return false;
		DrawFilledRect(rail_x, rail_y + 4u, rail_width, 5u, rail);

		const float range = std::max(max_value - min_value, 0.001f);
		const double mouse_x = extent_.width == 0u ? 0.0 : frame.input.mouse_x * (static_cast<double>(render_extent_.width) / static_cast<double>(extent_.width));
		const double mouse_y = extent_.height == 0u ? 0.0 : frame.input.mouse_y * (static_cast<double>(render_extent_.height) / static_cast<double>(extent_.height));
		const bool hovered = mouse_x >= rail_x && mouse_x <= static_cast<double>(rail_x + rail_width) &&
			mouse_y >= rail_y && mouse_y <= static_cast<double>(rail_y + 14u);
		bool changed = false;
		if (hovered && frame.input.mouse_left_down)
		{
			const float t = static_cast<float>((mouse_x - static_cast<double>(rail_x)) / static_cast<double>(rail_width));
			value = min_value + (std::clamp(t, 0.0f, 1.0f) * range);
			changed = true;
		}

		const float normalized = std::clamp((value - min_value) / range, 0.0f, 1.0f);
		const std::uint32_t filled = static_cast<std::uint32_t>(normalized * static_cast<float>(rail_width));
		DrawFilledRect(rail_x, rail_y + 4u, filled, 5u, fill);
		const std::uint32_t knob_x = rail_x + std::min(filled, rail_width > 1u ? rail_width - 1u : 0u);
		DrawFilledRect(knob_x > 2u ? knob_x - 2u : knob_x, rail_y, 5u, 13u, handle);

		std::array<char, 24> value_text{};
		std::snprintf(value_text.data(), value_text.size(), "%.1f", value);
		DrawText(value_text.data(), rail_x + rail_width + 12u, y, 1u, text);
		return changed;
	}

	void VulkanSoftwareVoxelRasterizer::DrawTuningPanel(const VulkanSoftwareVoxelRasterizerFrame& frame)
	{
		const std::uint32_t panel_width = std::min<std::uint32_t>(520u, render_extent_.width > 24u ? render_extent_.width - 24u : render_extent_.width);
		const std::uint32_t panel_height = 150u;
		const std::uint32_t panel_x = render_extent_.width > panel_width + 20u ? render_extent_.width - panel_width - 20u : 0u;
		const std::uint32_t panel_y = render_extent_.height > panel_height + 20u ? 20u : 0u;
		const std::uint32_t bg = PackColor({ 16, 24, 32 }, frame.format);
		const std::uint32_t border = PackColor({ 87, 220, 180 }, frame.format);
		const std::uint32_t text = PackColor({ 232, 248, 244 }, frame.format);
		DrawFilledRect(panel_x, panel_y, panel_width, panel_height, bg);
		DrawFilledRect(panel_x, panel_y, panel_width, 3u, border);
		DrawFilledRect(panel_x, panel_y + panel_height - 3u, panel_width, 3u, border);
		DrawFilledRect(panel_x, panel_y, 3u, panel_height, border);
		DrawFilledRect(panel_x + panel_width - 3u, panel_y, 3u, panel_height, border);
		DrawText("VULKAN DEMO TUNING", panel_x + 12u, panel_y + 10u, 1u, text);

		float res_x = static_cast<float>(frame.settings.max_internal_width);
		float res_y = static_cast<float>(frame.settings.max_internal_height);
		float pixel = static_cast<float>(frame.settings.pixel_block_size);
		float ray = frame.settings.max_ray_distance;
		float fog = frame.settings.fog_strength;
		float outline = frame.settings.outline_strength;
		const std::uint32_t slider_x = panel_x + 12u;
		const std::uint32_t slider_w = std::min<std::uint32_t>(220u, panel_width > 250u ? panel_width - 250u : 120u);
		Slider(frame, "RES X", res_x, 320.0f, 1920.0f, slider_x, panel_y + 31u, slider_w);
		Slider(frame, "RES Y", res_y, 180.0f, 1080.0f, slider_x, panel_y + 49u, slider_w);
		Slider(frame, "PIXEL", pixel, 1.0f, 8.0f, slider_x, panel_y + 67u, slider_w);
		Slider(frame, "RAY", ray, 32.0f, 144.0f, slider_x, panel_y + 85u, slider_w);
		Slider(frame, "FOG", fog, 0.0f, 0.9f, slider_x, panel_y + 103u, slider_w);
		Slider(frame, "OUTLINE", outline, 0.0f, 0.65f, slider_x, panel_y + 121u, slider_w);
		frame.settings.max_internal_width = static_cast<std::uint32_t>(std::clamp(res_x, 320.0f, 1920.0f));
		frame.settings.max_internal_height = static_cast<std::uint32_t>(std::clamp(res_y, 180.0f, 1080.0f));
		frame.settings.pixel_block_size = static_cast<std::uint32_t>(std::round(std::clamp(pixel, 1.0f, 8.0f)));
		frame.settings.max_ray_distance = std::clamp(ray, 32.0f, 144.0f);
		frame.settings.fog_strength = std::clamp(fog, 0.0f, 0.9f);
		frame.settings.outline_strength = std::clamp(outline, 0.0f, 0.65f);
	}

	void VulkanSoftwareVoxelRasterizer::DrawDemoOverlay(const VulkanSoftwareVoxelRasterizerFrame& frame)
	{
		const std::uint32_t accent = PackColor({ 87, 220, 180 }, frame.format);
		const std::uint32_t shadow = PackColor({ 18, 32, 44 }, frame.format);
		const std::uint32_t text = PackColor({ 232, 248, 244 }, frame.format);
		const std::uint32_t desired_bar_width = render_extent_.width > 480u ? std::min<std::uint32_t>(render_extent_.width / 3u, 430u) : render_extent_.width;
		const std::uint32_t bar_y = render_extent_.height > 32u ? 14u : 0u;
		const std::uint32_t bar_x = std::min<std::uint32_t>(20u, render_extent_.width);
		const std::uint32_t bar_width = std::min(desired_bar_width, render_extent_.width - bar_x);
		const std::uint32_t bar_end_x = std::min(bar_x + bar_width, render_extent_.width);
		for (std::uint32_t y = bar_y; y < bar_y + 96u && y < render_extent_.height; ++y)
		{
			std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(y) * render_extent_.width);
			std::fill(row + bar_x, row + bar_end_x, shadow);
		}
		for (std::uint32_t y = bar_y; y < bar_y + 4u && y < render_extent_.height; ++y)
		{
			std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(y) * render_extent_.width);
			std::fill(row + bar_x, row + bar_end_x, accent);
		}

		std::array<char, 64> fps_line{};
		std::array<char, 64> cpu_line{};
		std::array<char, 64> vk_line{};
		std::array<char, 64> resolution_line{};
		std::array<char, 64> sampling_line{};
		const double milliseconds = std::max(frame.delta_seconds, 0.0) * 1000.0;
		if (frame.displayed_fps > 0)
		{
			std::snprintf(fps_line.data(), fps_line.size(), "FPS %d   %.1f MS", frame.displayed_fps, milliseconds);
		}
		else
		{
			std::snprintf(fps_line.data(), fps_line.size(), "FPS --   %.1f MS", milliseconds);
		}
		std::snprintf(cpu_line.data(), cpu_line.size(), "SNAP %.1f RAST %.1f UI %.1f", frame.previous_timing.snapshot_cpu_ms, frame.previous_timing.raster_cpu_ms, frame.previous_timing.upscale_cpu_ms);
		std::snprintf(vk_line.data(), vk_line.size(), "COPY %.1f VK %.1f", frame.previous_timing.upload_cpu_ms, frame.previous_timing.present_cpu_ms);
		std::snprintf(resolution_line.data(), resolution_line.size(), "OUT %uX%u IN %uX%u",
			extent_.width,
			extent_.height,
			frame.previous_timing.render_extent.width,
			frame.previous_timing.render_extent.height);
		std::snprintf(sampling_line.data(), sampling_line.size(), "STEP %u TH %u",
			frame.previous_timing.sample_step,
			frame.previous_timing.worker_count);

		DrawText("VULKAN VOXEL DEMO", bar_x + 8u, bar_y + 9u, 1u, text);
		DrawText(fps_line.data(), bar_x + 8u, bar_y + 23u, 1u, text);
		DrawText(cpu_line.data(), bar_x + 8u, bar_y + 37u, 1u, text);
		DrawText(vk_line.data(), bar_x + 8u, bar_y + 51u, 1u, text);
		if (frame.previous_timing.has_gpu_copy_timing)
		{
			std::array<char, 64> gpu_line{};
			std::snprintf(gpu_line.data(), gpu_line.size(), "GPU COPY %.2f", frame.previous_timing.gpu_copy_ms);
			DrawText(gpu_line.data(), bar_x + 8u, bar_y + 65u, 1u, text);
		}
		else
		{
			DrawText("GPU COPY --", bar_x + 8u, bar_y + 65u, 1u, text);
		}
		DrawText(resolution_line.data(), bar_x + 8u, bar_y + 79u, 1u, text);
		DrawText(sampling_line.data(), bar_x + 8u, bar_y + 93u, 1u, text);
	}
}
