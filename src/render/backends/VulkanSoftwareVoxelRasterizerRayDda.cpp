#include "VulkanSoftwareVoxelRasterizerRaycast.h"

#include "VulkanSoftwareVoxelRasterizerRayTraversal.h"

namespace ve::rendering
{
	namespace
	{
		constexpr int kMaxRaySteps = 256;
	}

	glm::vec3 FindAirCameraOrigin(const VulkanRasterFrameWorldSnapshot& world, glm::vec3 origin)
	{
		for (int attempts = 0; attempts < 32; ++attempts)
		{
			const ve::blocks::BlockId camera_block =
				world.GetBlock({
					raster_ray::FloorBlockCoordinate(origin.x),
					raster_ray::FloorBlockCoordinate(origin.y),
					raster_ray::FloorBlockCoordinate(origin.z)
				});
			if (!raster_ray::IsSolidBlock(camera_block)) return origin;
			origin.y += 1.0f;
		}
		return origin;
	}

	VulkanRasterRayHit TraceWorld(const VulkanRasterFrameWorldSnapshot& world, const glm::vec3& origin, const glm::vec3& direction, float max_ray_distance)
	{
		raster_ray::TraversalState traversal = raster_ray::StartTraversal(origin, direction);

		for (int ray_step = 0; ray_step < kMaxRaySteps && traversal.traveled_distance <= max_ray_distance; ++ray_step)
		{
			if (raster_ray::IsInsideWorld(traversal.current_block, world))
			{
				const ve::blocks::BlockId block_id = world.GetBlock(traversal.current_block);
				if (raster_ray::IsSolidBlock(block_id))
				{
					return {
						true,
						block_id,
						traversal.current_block,
						traversal.surface_normal,
						traversal.traveled_distance
					};
				}
			}
			else if (raster_ray::IsLeavingWorld(traversal.current_block, traversal.block_step, world)) return {};

			raster_ray::AdvanceToNextBlock(traversal);
		}
		return {};
	}
}
