#include "VulkanSoftwareVoxelRasterizer.h"

#include "World.h"

namespace ve::rendering
{
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
}
