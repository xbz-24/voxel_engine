#include "VulkanSoftwareVoxelRasterizerRayTraversal.h"

#include "BlockDefinitions.h"

#include <cmath>
#include <cstddef>
#include <limits>

namespace ve::rendering::raster_ray
{
	namespace
	{
		[[nodiscard]] int StepDirection(float direction) noexcept
		{
			if (direction > 0.0f) return 1;
			if (direction < 0.0f) return -1;
			return 0;
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
	}

	bool IsSolidBlock(ve::blocks::BlockId block) noexcept
	{
		const auto block_index = static_cast<std::size_t>(block);
		return block_index < static_cast<std::size_t>(ve::blocks::BlockId::Count) &&
			ve::blocks::BuiltInBlockDefinitions[block_index].isSolid;
	}

	int FloorBlockCoordinate(float value) noexcept
	{
		return static_cast<int>(std::floor(value));
	}

	bool IsInsideWorld(const glm::ivec3& block, const VulkanRasterFrameWorldSnapshot& world) noexcept
	{
		return block.x >= 0 && block.y >= 0 && block.z >= 0 &&
			block.x < world.width && block.y < world.height && block.z < world.depth;
	}

	bool IsLeavingWorld(
		const glm::ivec3& block,
		const glm::ivec3& block_step,
		const VulkanRasterFrameWorldSnapshot& world) noexcept
	{
		return (block.x < 0 && block_step.x <= 0) || (block.x >= world.width && block_step.x >= 0) ||
			(block.y < 0 && block_step.y <= 0) || (block.y >= world.height && block_step.y >= 0) ||
			(block.z < 0 && block_step.z <= 0) || (block.z >= world.depth && block_step.z >= 0);
	}

	TraversalState StartTraversal(const glm::vec3& origin, const glm::vec3& direction) noexcept
	{
		return {
			.current_block = { FloorBlockCoordinate(origin.x), FloorBlockCoordinate(origin.y), FloorBlockCoordinate(origin.z) },
			.block_step = { StepDirection(direction.x), StepDirection(direction.y), StepDirection(direction.z) },
			.next_axis_distance = {
				InitialAxisDistance(origin.x, direction.x),
				InitialAxisDistance(origin.y, direction.y),
				InitialAxisDistance(origin.z, direction.z)
			},
			.axis_distance_delta = { AxisDelta(direction.x), AxisDelta(direction.y), AxisDelta(direction.z) }
		};
	}

	void AdvanceToNextBlock(TraversalState& traversal) noexcept
	{
		if (traversal.next_axis_distance.x < traversal.next_axis_distance.y &&
			traversal.next_axis_distance.x < traversal.next_axis_distance.z)
		{
			traversal.current_block.x += traversal.block_step.x;
			traversal.traveled_distance = traversal.next_axis_distance.x;
			traversal.next_axis_distance.x += traversal.axis_distance_delta.x;
			traversal.surface_normal = { -traversal.block_step.x, 0, 0 };
		}
		else if (traversal.next_axis_distance.y < traversal.next_axis_distance.z)
		{
			traversal.current_block.y += traversal.block_step.y;
			traversal.traveled_distance = traversal.next_axis_distance.y;
			traversal.next_axis_distance.y += traversal.axis_distance_delta.y;
			traversal.surface_normal = { 0, -traversal.block_step.y, 0 };
		}
		else
		{
			traversal.current_block.z += traversal.block_step.z;
			traversal.traveled_distance = traversal.next_axis_distance.z;
			traversal.next_axis_distance.z += traversal.axis_distance_delta.z;
			traversal.surface_normal = { 0, 0, -traversal.block_step.z };
		}
	}
}
