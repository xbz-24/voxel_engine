#include "VulkanSoftwareVoxelRasterizer.h"

#include "World.h"

namespace ve::rendering
{
	void VulkanSoftwareVoxelRasterizer::CaptureWorldSnapshot(const ve::world::World& world)
	{
		const ve::world::WorldMetrics metrics = world.Metrics();
		world_snapshot_.width = metrics.worldSizeInChunks * ve::world::terrain::ChunkWidth;
		world_snapshot_.height = ve::world::terrain::ChunkHeight;
		world_snapshot_.depth = metrics.worldSizeInChunks * ve::world::terrain::ChunkDepth;

		const std::size_t block_count =
			static_cast<std::size_t>(world_snapshot_.width) *
			static_cast<std::size_t>(world_snapshot_.height) *
			static_cast<std::size_t>(world_snapshot_.depth);
		world_snapshot_.blocks.resize(block_count);

		std::size_t snapshot_block_index = 0;
		for (int block_x = 0; block_x < world_snapshot_.width; ++block_x)
		{
			for (int block_y = 0; block_y < world_snapshot_.height; ++block_y)
			{
				for (int block_z = 0; block_z < world_snapshot_.depth; ++block_z)
				{
					world_snapshot_.blocks[snapshot_block_index++] = world.GetBlock(block_x, block_y, block_z);
				}
			}
		}
		world_snapshot_.world_revision = world.Revision();
	}
}
