#include "VulkanSoftwareVoxelRasterizerRayTraversal.h"

#include "BlockDefinitions.h"
#include "CoreTypes.h"

#include <cmath>

namespace ve::rendering::raster_ray
{
	bool IsSolidBlock(ve::blocks::BlockId block) noexcept
	{
		const ve::blocks::BlockOrdinal block_index = ve::blocks::ToBlockOrdinal(block);
		return block_index < ve::blocks::BlockTypeCount &&
			ve::blocks::BuiltInBlockDefinitions[block_index].isSolid;
	}

	int FloorBlockCoordinate(float value) noexcept
	{
		return ve::core::ToInt(std::floor(value));
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
