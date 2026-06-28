#include "VulkanGpuChunkRenderer.h"

#include "World.h"

#include <algorithm>

namespace ve::rendering
{
	void VulkanGpuChunkRenderer::RebuildMesh(const ve::world::World& world, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const
	{
		// TODO: Replace full-world rebuilds with dirty chunk meshes cached across World::Revision() changes.
		const ve::world::WorldMetrics metrics = world.Metrics();
		const int width = metrics.worldSizeChunks * Chunk::CHUNK_WIDTH;
		const int height = Chunk::CHUNK_HEIGHT;
		const int depth = metrics.worldSizeChunks * Chunk::CHUNK_DEPTH;
		vertices.clear();
		indices.clear();
		const std::size_t column_count = static_cast<std::size_t>(std::max(width, 0)) * static_cast<std::size_t>(std::max(depth, 0));
		vertices.reserve(std::max<std::size_t>(140'000u, column_count * 16u));
		indices.reserve(std::max<std::size_t>(210'000u, column_count * 24u));

		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				for (int z = 0; z < depth; ++z)
				{
					const ve::blocks::BlockId block = world.GetBlock(x, y, z);
					if (!IsRenderableBlock(block)) continue;
					AppendVisibleBlockFaces(world, x, y, z, block, vertices, indices);
				}
			}
		}
	}
	void VulkanGpuChunkRenderer::AppendVisibleBlockFaces(const ve::world::World& world,
		int x,
		int y,
		int z,
		ve::blocks::BlockId block,
		std::vector<VoxelVertex>& vertices,
		std::vector<std::uint32_t>& indices) const
	{
		for (const BlockFaceGeometry& face : ChunkFaces())
		{
			const ve::blocks::BlockId neighbor = world.GetBlock(x + face.neighbor_offset.x, y + face.neighbor_offset.y, z + face.neighbor_offset.z);
			if (!OccludesNeighborFaces(neighbor))
			{
				AppendFaceMesh(face, world, x, y, z, block, vertices, indices);
			}
		}
	}
}
