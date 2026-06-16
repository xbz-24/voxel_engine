#include "VulkanSoftwareVoxelRasterizer.h"

#include "BlockDefinitions.h"
#include "Camera.h"
#include "World.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>

namespace ve::rendering
{
	namespace
	{
		struct Rgb
		{
			std::uint8_t r;
			std::uint8_t g;
			std::uint8_t b;
		};

		struct RayHit
		{
			bool hit = false;
			ve::blocks::BlockId block = ve::blocks::BlockId::Air;
			glm::ivec3 normal{ 0, 1, 0 };
			float distance = 0.0f;
		};

		constexpr float kMaxRayDistance = 150.0f;
		constexpr int kMaxRaySteps = 256;

		bool IsBgraFormat(VkFormat format) noexcept
		{
			return format == VK_FORMAT_B8G8R8A8_UNORM ||
				format == VK_FORMAT_B8G8R8A8_SRGB ||
				format == VK_FORMAT_B8G8R8A8_SNORM;
		}

		std::uint32_t PackColor(Rgb color, VkFormat format) noexcept
		{
			constexpr std::uint32_t alpha = 255u;
			if (IsBgraFormat(format))
			{
				return (alpha << 24u) |
					(static_cast<std::uint32_t>(color.r) << 16u) |
					(static_cast<std::uint32_t>(color.g) << 8u) |
					static_cast<std::uint32_t>(color.b);
			}
			return (alpha << 24u) |
				(static_cast<std::uint32_t>(color.b) << 16u) |
				(static_cast<std::uint32_t>(color.g) << 8u) |
				static_cast<std::uint32_t>(color.r);
		}

		std::uint8_t Scale(std::uint8_t value, float amount) noexcept
		{
			const float scaled = std::clamp(static_cast<float>(value) * amount, 0.0f, 255.0f);
			return static_cast<std::uint8_t>(scaled);
		}

		Rgb Mix(Rgb left, Rgb right, float amount) noexcept
		{
			const float t = std::clamp(amount, 0.0f, 1.0f);
			const auto lerp = [t](std::uint8_t a, std::uint8_t b)
			{
				return static_cast<std::uint8_t>((static_cast<float>(a) * (1.0f - t)) + (static_cast<float>(b) * t));
			};
			return { lerp(left.r, right.r), lerp(left.g, right.g), lerp(left.b, right.b) };
		}

		Rgb SkyColor(float direction_y) noexcept
		{
			const float t = std::clamp((direction_y + 0.20f) * 0.80f, 0.0f, 1.0f);
			return Mix({ 78, 107, 150 }, { 166, 210, 255 }, t);
		}

		bool IsSolid(ve::blocks::BlockId block) noexcept
		{
			const auto index = static_cast<std::size_t>(block);
			return index < static_cast<std::size_t>(ve::blocks::BlockId::Count) &&
				ve::blocks::BuiltInBlockDefinitions[index].isSolid;
		}

		Rgb BaseBlockColor(ve::blocks::BlockId block) noexcept
		{
			using enum ve::blocks::BlockId;
			if (block == Grass) return { 92, 159, 74 };
			if (block == Dirt) return { 112, 76, 45 };
			if (block == Stone) return { 124, 124, 124 };
			if (block == Cobblestone) return { 105, 105, 105 };
			if (block == OakLog || block == BirchLog || block == SpruceLog) return { 123, 89, 50 };
			if (block == OakPlanks || block == BirchPlanks || block == SprucePlanks) return { 174, 134, 82 };
			if (block == Sand) return { 214, 198, 129 };
			if (block == Bricks) return { 151, 74, 60 };
			if (block == DiamondOre) return { 92, 196, 211 };
			if (block == CoalOre) return { 54, 54, 54 };
			if (block == IronOre) return { 216, 170, 130 };
			if (block == Obsidian) return { 42, 32, 65 };
			if (block == OakLeaves || block == BirchLeaves) return { 67, 137, 58 };
			if (block == Snow) return { 235, 242, 246 };
			if (block == MossBlock) return { 73, 126, 58 };
			if (block == Pumpkin) return { 198, 112, 33 };
			if (block == Melon) return { 104, 158, 53 };
			if (block == HayBlock) return { 203, 174, 63 };
			if (block == AmethystBlock) return { 137, 89, 178 };
			if (block == Glass) return { 148, 214, 219 };
			return { 132, 132, 132 };
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

		glm::vec3 FindAirCameraOrigin(const ve::world::World& world, glm::vec3 origin)
		{
			for (int attempts = 0; attempts < 32; ++attempts)
			{
				const auto block = world.GetBlock(FloorToInt(origin.x), FloorToInt(origin.y), FloorToInt(origin.z));
				if (!IsSolid(block)) return origin;
				origin.y += 1.0f;
			}
			return origin;
		}

		RayHit TraceWorld(const ve::world::World& world, const glm::vec3& origin, const glm::vec3& direction)
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

			for (int ray_step = 0; ray_step < kMaxRaySteps && distance <= kMaxRayDistance; ++ray_step)
			{
				const ve::blocks::BlockId block_id = world.GetBlock(block);
				if (IsSolid(block_id)) return { true, block_id, normal, distance };

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

		Rgb ShadeHit(const RayHit& hit, const glm::vec3& direction) noexcept
		{
			float shade = 0.72f;
			if (hit.normal.y > 0) shade = 1.05f;
			else if (hit.normal.y < 0) shade = 0.45f;
			else if (hit.normal.x != 0) shade = 0.78f;

			Rgb color = BaseBlockColor(hit.block);
			color = { Scale(color.r, shade), Scale(color.g, shade), Scale(color.b, shade) };
			const float fog = std::clamp(hit.distance / kMaxRayDistance, 0.0f, 1.0f);
			return Mix(color, SkyColor(direction.y), fog * 0.55f);
		}

		std::uint32_t SampleStepFor(VkExtent2D extent) noexcept
		{
			const std::uint64_t pixel_count = static_cast<std::uint64_t>(extent.width) * static_cast<std::uint64_t>(extent.height);
			if (pixel_count > (1920ull * 1080ull)) return 6u;
			if (pixel_count > (1280ull * 720ull)) return 5u;
			if (pixel_count > (960ull * 540ull)) return 3u;
			return 2u;
		}

		bool NearlyEqual(const glm::vec3& left, const glm::vec3& right) noexcept
		{
			constexpr float kEpsilon = 0.0001f;
			return glm::all(glm::lessThan(glm::abs(left - right), glm::vec3{ kEpsilon }));
		}
	}

	bool VulkanSoftwareVoxelRasterizer::Resize(VkExtent2D extent)
	{
		if (extent.width == 0 || extent.height == 0) return false;
		if (extent_.width == extent.width && extent_.height == extent.height) return true;
		extent_ = extent;
		pixels_.assign(static_cast<std::size_t>(extent.width) * static_cast<std::size_t>(extent.height), 0u);
		ray_cache_valid_ = false;
		return true;
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

		const float width = static_cast<float>(extent_.width);
		const float height = static_cast<float>(extent_.height);
		const float aspect = width / height;
		const float tan_half_fov = std::tan(glm::radians(35.0f));
		ray_cache_.clear();
		const std::size_t sample_columns = (extent_.width + sample_step - 1u) / sample_step;
		const std::size_t sample_rows = (extent_.height + sample_step - 1u) / sample_step;
		ray_cache_.reserve(sample_columns * sample_rows);

		for (std::uint32_t y = 0; y < extent_.height; y += sample_step)
		{
			for (std::uint32_t x = 0; x < extent_.width; x += sample_step)
			{
				const float screen_x = ((static_cast<float>(x) + 0.5f) / width) * 2.0f - 1.0f;
				const float screen_y = 1.0f - ((static_cast<float>(y) + 0.5f) / height) * 2.0f;
				ray_cache_.push_back({
					x,
					y,
					std::min(x + sample_step, extent_.width),
					std::min(y + sample_step, extent_.height),
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

	void VulkanSoftwareVoxelRasterizer::Render(const VulkanSoftwareVoxelRasterizerFrame& frame)
	{
		if (!Resize(frame.extent)) return;

		const glm::vec3 origin = FindAirCameraOrigin(frame.world, frame.camera.GetPosition());
		const std::uint32_t sample_step = SampleStepFor(extent_);
		EnsureRayCache(frame.camera, sample_step);

		for (const CachedSampleRay& sample : ray_cache_)
		{
			const RayHit hit = TraceWorld(frame.world, origin, sample.direction);
			const Rgb color = hit.hit ? ShadeHit(hit, sample.direction) : SkyColor(sample.direction.y);
			const std::uint32_t packed = PackColor(color, frame.format);
			for (std::uint32_t fill_y = sample.y; fill_y < sample.y_end; ++fill_y)
			{
				std::uint32_t* row = pixels_.data() + (static_cast<std::size_t>(fill_y) * extent_.width);
				std::fill(row + sample.x, row + sample.x_end, packed);
			}
		}
		DrawDemoOverlay(frame.format);
		DrawCrosshair(frame.format);
	}

	std::span<const std::uint32_t> VulkanSoftwareVoxelRasterizer::Pixels() const noexcept
	{
		return pixels_;
	}

	VkExtent2D VulkanSoftwareVoxelRasterizer::Extent() const noexcept
	{
		return extent_;
	}

	void VulkanSoftwareVoxelRasterizer::DrawCrosshair(VkFormat format)
	{
		const std::uint32_t center_x = extent_.width / 2u;
		const std::uint32_t center_y = extent_.height / 2u;
		const std::uint32_t crosshair = PackColor({ 235, 235, 235 }, format);
		for (std::uint32_t offset = 0; offset < 8u; ++offset)
		{
			if (center_x + offset < extent_.width) pixels_[(center_y * extent_.width) + center_x + offset] = crosshair;
			if (center_x >= offset) pixels_[(center_y * extent_.width) + center_x - offset] = crosshair;
			if (center_y + offset < extent_.height) pixels_[((center_y + offset) * extent_.width) + center_x] = crosshair;
			if (center_y >= offset) pixels_[((center_y - offset) * extent_.width) + center_x] = crosshair;
		}
	}

	void VulkanSoftwareVoxelRasterizer::DrawDemoOverlay(VkFormat format)
	{
		const std::uint32_t sky_line = extent_.height / 2u;
		const std::uint32_t horizon = PackColor({ 214, 225, 230 }, format);
		for (std::uint32_t x = 0; x < extent_.width; x += 4u)
		{
			if (sky_line < extent_.height) pixels_[(sky_line * extent_.width) + x] = horizon;
		}

		const std::uint32_t accent = PackColor({ 87, 220, 180 }, format);
		const std::uint32_t shadow = PackColor({ 18, 32, 44 }, format);
		const std::uint32_t bar_width = std::min<std::uint32_t>(extent_.width / 5u, 260u);
		const std::uint32_t bar_y = extent_.height > 32u ? 24u : 0u;
		const std::uint32_t bar_x = std::min<std::uint32_t>(20u, extent_.width);
		const std::uint32_t bar_end_x = std::min(bar_x + bar_width, extent_.width);
		for (std::uint32_t y = bar_y; y < bar_y + 6u && y < extent_.height; ++y)
		{
			std::uint32_t* row = pixels_.data() + (static_cast<std::size_t>(y) * extent_.width);
			std::fill(row + bar_x, row + bar_end_x, y == bar_y ? accent : shadow);
		}
	}
}
