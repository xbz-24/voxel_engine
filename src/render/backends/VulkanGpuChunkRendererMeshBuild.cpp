#include "VulkanGpuChunkRenderer.h"

#include "World.h"

#include <algorithm>

namespace ve::rendering
{
	void VulkanGpuChunkRendererMeshOperations::RebuildChunkMesh(
		const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		const Chunk& chunk,
		VulkanGpuCachedChunkMesh& cached_mesh) const
	{
		const int chunk_origin_x = chunk.GetChunkX() * Chunk::CHUNK_WIDTH;
		const int chunk_origin_z = chunk.GetChunkZ() * Chunk::CHUNK_DEPTH;
		cached_mesh.vertices.clear();
		cached_mesh.indices.clear();
		constexpr std::size_t column_count = ChunkColumnCount(Chunk::CHUNK_WIDTH, Chunk::CHUNK_DEPTH);
		cached_mesh.vertices.reserve(std::max<std::size_t>(2'048u, column_count * 16u));
		cached_mesh.indices.reserve(std::max<std::size_t>(3'072u, column_count * 24u));

		for (int local_x = 0; local_x < Chunk::CHUNK_WIDTH; ++local_x)
		{
			const int block_x = chunk_origin_x + local_x;
			for (int block_y = 0; block_y < Chunk::CHUNK_HEIGHT; ++block_y)
			{
				for (int local_z = 0; local_z < Chunk::CHUNK_DEPTH; ++local_z)
				{
					const ve::blocks::BlockId block = chunk.GetBlock(local_x, block_y, local_z);
					if (!IsRenderableBlock(block_registry, block)) continue;
					AppendVisibleBlockFaces(world, block_registry, block_x, block_y,
						chunk_origin_z + local_z, block, cached_mesh.vertices, cached_mesh.indices);
				}
			}
		}
		cached_mesh.mesh_revision = chunk.MeshRevision();
	}

	void VulkanGpuChunkRendererMeshOperations::RebuildMesh(
		const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		std::vector<VoxelVertex>& vertices,
		std::vector<std::uint32_t>& indices)
	{
		ResetChunkMeshCacheForWorldStorage(world);
		const ve::world::WorldMetrics metrics = world.Metrics();
		const int width = metrics.worldSizeInChunks * Chunk::CHUNK_WIDTH;
		const int depth = metrics.worldSizeInChunks * Chunk::CHUNK_DEPTH;
		vertices.clear();
		indices.clear();
		last_rebuilt_chunk_count_ = 0;
		const std::size_t column_count = WorldColumnCount(width, depth);
		vertices.reserve(std::max<std::size_t>(140'000u, column_count * 16u));
		indices.reserve(std::max<std::size_t>(210'000u, column_count * 24u));

		for (const Chunk& chunk : world.Chunks())
		{
			VulkanGpuCachedChunkMesh& cached_mesh = CachedMeshFor(chunk);
			if (cached_mesh.mesh_revision != chunk.MeshRevision())
			{
				RebuildChunkMesh(world, block_registry, chunk, cached_mesh);
				++last_rebuilt_chunk_count_;
			}
			AppendCachedChunkMesh(cached_mesh, vertices, indices);
		}
	}
}
