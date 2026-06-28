#include "VulkanSoftwareVoxelRasterizerRaycast.h"

#include "BlockDefinitions.h"

#include <cmath>
#include <cstddef>
#include <limits>

namespace ve::rendering
{
	namespace
	{
		constexpr int kMaxRaySteps = 256;

		[[nodiscard]] bool IsSolid(ve::blocks::BlockId block) noexcept
		{
			const auto index = static_cast<std::size_t>(block);
			return index < static_cast<std::size_t>(ve::blocks::BlockId::Count) &&
				ve::blocks::BuiltInBlockDefinitions[index].isSolid;
		}

		[[nodiscard]] int FloorToInt(float value) noexcept
		{
			return static_cast<int>(std::floor(value));
		}

		[[nodiscard]] float InitialAxisDistance(float origin, float direction) noexcept
		{
			if (direction > 0.0f) return (std::floor(origin) + 1.0f - origin) / direction;
			if (direction < 0.0f) return (origin - std::floor(origin)) / -direction;
			return std::numeric_limits<float>::infinity();
		}

		[[nodiscard]] float AxisDelta(float direction) noexcept
		{
			return direction == 0.0f ? std::numeric_limits<float>::infinity() : std::abs(1.0f / direction);
		}

		[[nodiscard]] bool LeavingWorld(const glm::ivec3& block, const glm::ivec3& step, const VulkanRasterFrameWorldSnapshot& world) noexcept
		{
			return (block.x < 0 && step.x <= 0) || (block.x >= world.width && step.x >= 0) ||
				(block.y < 0 && step.y <= 0) || (block.y >= world.height && step.y >= 0) ||
				(block.z < 0 && step.z <= 0) || (block.z >= world.depth && step.z >= 0);
		}

		[[nodiscard]] bool InsideWorld(const glm::ivec3& block, const VulkanRasterFrameWorldSnapshot& world) noexcept
		{
			return block.x >= 0 && block.y >= 0 && block.z >= 0 &&
				block.x < world.width && block.y < world.height && block.z < world.depth;
		}
	}

	glm::vec3 FindAirCameraOrigin(const VulkanRasterFrameWorldSnapshot& world, glm::vec3 origin)
	{
		for (int attempts = 0; attempts < 32; ++attempts)
		{
			const auto block = world.GetBlock({ FloorToInt(origin.x), FloorToInt(origin.y), FloorToInt(origin.z) });
			if (!IsSolid(block)) return origin;
			origin.y += 1.0f;
		}
		return origin;
	}

	VulkanRasterRayHit TraceWorld(const VulkanRasterFrameWorldSnapshot& world, const glm::vec3& origin, const glm::vec3& direction, float max_ray_distance)
	{
		glm::ivec3 block{ FloorToInt(origin.x), FloorToInt(origin.y), FloorToInt(origin.z) };
		const glm::ivec3 step{ direction.x > 0.0f ? 1 : (direction.x < 0.0f ? -1 : 0),
			direction.y > 0.0f ? 1 : (direction.y < 0.0f ? -1 : 0),
			direction.z > 0.0f ? 1 : (direction.z < 0.0f ? -1 : 0) };
		glm::vec3 max_distance{ InitialAxisDistance(origin.x, direction.x), InitialAxisDistance(origin.y, direction.y), InitialAxisDistance(origin.z, direction.z) };
		const glm::vec3 delta{ AxisDelta(direction.x), AxisDelta(direction.y), AxisDelta(direction.z) };
		glm::ivec3 normal{ 0, 1, 0 };
		float distance = 0.0f;

		for (int ray_step = 0; ray_step < kMaxRaySteps && distance <= max_ray_distance; ++ray_step)
		{
			if (InsideWorld(block, world))
			{
				const ve::blocks::BlockId block_id = world.GetBlock(block);
				if (IsSolid(block_id)) return { true, block_id, block, normal, distance };
			}
			else if (LeavingWorld(block, step, world)) return {};

			if (max_distance.x < max_distance.y && max_distance.x < max_distance.z)
			{
				block.x += step.x; distance = max_distance.x; max_distance.x += delta.x; normal = { -step.x, 0, 0 };
			}
			else if (max_distance.y < max_distance.z)
			{
				block.y += step.y; distance = max_distance.y; max_distance.y += delta.y; normal = { 0, -step.y, 0 };
			}
			else
			{
				block.z += step.z; distance = max_distance.z; max_distance.z += delta.z; normal = { 0, 0, -step.z };
			}
		}
		return {};
	}
}
