#include "VulkanGpuChunkRenderer.h"

#include "World.h"

#include <algorithm>
#include <ranges>

namespace ve::rendering
{
	void VulkanGpuChunkRenderer::ResetChunkMeshCacheForWorldStorage(const ve::world::World& world)
	{
		if (cached_chunk_storage_revision_ == world.ChunkStorageRevision())
		{
			return;
		}
		cached_chunk_meshes_.clear();
		cached_chunk_storage_revision_ = world.ChunkStorageRevision();
	}

	VulkanGpuChunkRenderer::CachedChunkMesh& VulkanGpuChunkRenderer::CachedMeshFor(const Chunk& chunk)
	{
		const auto cached_mesh = std::ranges::find_if(cached_chunk_meshes_,
			[&chunk](const CachedChunkMesh& candidate) noexcept {
				return candidate.chunk_x == chunk.GetChunkX() && candidate.chunk_z == chunk.GetChunkZ();
			});
		if (cached_mesh != cached_chunk_meshes_.end())
		{
			return *cached_mesh;
		}

		cached_chunk_meshes_.push_back(CachedChunkMesh{
			chunk.GetChunkX(),
			chunk.GetChunkZ()
		});
		return cached_chunk_meshes_.back();
	}

	void VulkanGpuChunkRenderer::RebuildChunkMesh(
		const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		const Chunk& chunk,
		CachedChunkMesh& cached_mesh) const
	{
		const int world_height_blocks = Chunk::CHUNK_HEIGHT;
		const int chunk_origin_x = chunk.GetChunkX() * Chunk::CHUNK_WIDTH;
		const int chunk_origin_z = chunk.GetChunkZ() * Chunk::CHUNK_DEPTH;
		cached_mesh.vertices.clear();
		cached_mesh.indices.clear();
		constexpr std::size_t chunk_column_count = static_cast<std::size_t>(Chunk::CHUNK_WIDTH * Chunk::CHUNK_DEPTH);
		cached_mesh.vertices.reserve(std::max<std::size_t>(2'048u, chunk_column_count * 16u));
		cached_mesh.indices.reserve(std::max<std::size_t>(3'072u, chunk_column_count * 24u));

		for (int local_block_x = 0; local_block_x < Chunk::CHUNK_WIDTH; ++local_block_x)
		{
			const int block_x = chunk_origin_x + local_block_x;
			for (int block_y = 0; block_y < world_height_blocks; ++block_y)
			{
				for (int local_block_z = 0; local_block_z < Chunk::CHUNK_DEPTH; ++local_block_z)
				{
					const ve::blocks::BlockId block = chunk.GetBlock(local_block_x, block_y, local_block_z);
					if (!IsRenderableBlock(block_registry, block)) continue;
					const int block_z = chunk_origin_z + local_block_z;
					AppendVisibleBlockFaces(world, block_registry, block_x, block_y, block_z, block, cached_mesh.vertices, cached_mesh.indices);
				}
			}
		}
		cached_mesh.mesh_revision = chunk.MeshRevision();
	}

	void VulkanGpuChunkRenderer::AppendCachedChunkMesh(
		const CachedChunkMesh& cached_mesh,
		std::vector<VoxelVertex>& vertices,
		std::vector<std::uint32_t>& indices) const
	{
		const std::uint32_t first_vertex_index = static_cast<std::uint32_t>(vertices.size());
		vertices.insert(vertices.end(), cached_mesh.vertices.begin(), cached_mesh.vertices.end());
		indices.reserve(indices.size() + cached_mesh.indices.size());
		for (std::uint32_t index : cached_mesh.indices)
		{
			indices.push_back(first_vertex_index + index);
		}
	}

	void VulkanGpuChunkRenderer::RebuildMesh(
		const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		std::vector<VoxelVertex>& vertices,
		std::vector<std::uint32_t>& indices)
	{
		ResetChunkMeshCacheForWorldStorage(world);
		const ve::world::WorldMetrics metrics = world.Metrics();
		const int world_width_blocks = metrics.worldSizeInChunks * Chunk::CHUNK_WIDTH;
		const int world_depth_blocks = metrics.worldSizeInChunks * Chunk::CHUNK_DEPTH;
		vertices.clear();
		indices.clear();
		last_rebuilt_chunk_count_ = 0;
		const std::size_t world_column_count = static_cast<std::size_t>(std::max(world_width_blocks, 0)) *
			static_cast<std::size_t>(std::max(world_depth_blocks, 0));
		vertices.reserve(std::max<std::size_t>(140'000u, world_column_count * 16u));
		indices.reserve(std::max<std::size_t>(210'000u, world_column_count * 24u));

		for (const Chunk& chunk : world.Chunks())
		{
			CachedChunkMesh& cached_mesh = CachedMeshFor(chunk);
			if (cached_mesh.mesh_revision != chunk.MeshRevision())
			{
				RebuildChunkMesh(world, block_registry, chunk, cached_mesh);
				++last_rebuilt_chunk_count_;
			}
			AppendCachedChunkMesh(cached_mesh, vertices, indices);
		}
	}

	void VulkanGpuChunkRenderer::AppendVisibleBlockFaces(const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
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
			if (!OccludesNeighborFaces(block_registry, neighbor_block))
			{
				AppendFaceMesh(face, world, block_registry, block_x, block_y, block_z, block, vertices, indices);
			}
		}
	}
}
