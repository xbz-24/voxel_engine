#include "VulkanGpuChunkRenderer.h"

#include "World.h"

#include <algorithm>

namespace ve::rendering
{
	void VulkanGpuChunkRenderer::RebuildMesh(const ve::world::World& world, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const
	{
		// TODO: Replace full-world rebuilds with dirty chunk meshes cached across World::Revision() changes.
		const ve::world::WorldMetrics metrics = world.Metrics();
		const int world_width_blocks = metrics.worldSizeInChunks * Chunk::CHUNK_WIDTH;
		const int world_height_blocks = Chunk::CHUNK_HEIGHT;
		const int world_depth_blocks = metrics.worldSizeInChunks * Chunk::CHUNK_DEPTH;
		vertices.clear();
		indices.clear();
		const std::size_t world_column_count = static_cast<std::size_t>(std::max(world_width_blocks, 0)) *
			static_cast<std::size_t>(std::max(world_depth_blocks, 0));
		vertices.reserve(std::max<std::size_t>(140'000u, world_column_count * 16u));
		indices.reserve(std::max<std::size_t>(210'000u, world_column_count * 24u));

		for (int block_x = 0; block_x < world_width_blocks; ++block_x)
		{
			for (int block_y = 0; block_y < world_height_blocks; ++block_y)
			{
				for (int block_z = 0; block_z < world_depth_blocks; ++block_z)
				{
					const ve::blocks::BlockId block = world.GetBlock(block_x, block_y, block_z);
					if (!IsRenderableBlock(block)) continue;
					AppendVisibleBlockFaces(world, block_x, block_y, block_z, block, vertices, indices);
				}
			}
		}
	}
	void VulkanGpuChunkRenderer::AppendVisibleBlockFaces(const ve::world::World& world,
		int block_x,
		int block_y,
		int block_z,
		ve::blocks::BlockId block,
		std::vector<VoxelVertex>& vertices,
		std::vector<std::uint32_t>& indices) const
	{
		for (const BlockFaceGeometry& face : ChunkFaces())
		{
			const ve::blocks::BlockId neighbor_block = world.GetBlock(
				block_x + face.neighbor_offset.x,
				block_y + face.neighbor_offset.y,
				block_z + face.neighbor_offset.z);
			if (!OccludesNeighborFaces(neighbor_block))
			{
				AppendFaceMesh(face, world, block_x, block_y, block_z, block, vertices, indices);
			}
		}
	}
}
