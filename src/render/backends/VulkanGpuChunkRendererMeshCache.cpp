#include "VulkanGpuChunkRenderer.h"

#include "World.h"

#include <ranges>

namespace ve::rendering
{
	void VulkanGpuChunkRendererMeshOperations::ResetChunkMeshCacheForWorldStorage(
		const ve::world::World& world)
	{
		if (cached_chunk_storage_revision_ == world.ChunkStorageRevision()) return;
		cached_chunk_meshes_.clear();
		cached_chunk_storage_revision_ = world.ChunkStorageRevision();
	}

	VulkanGpuCachedChunkMesh& VulkanGpuChunkRendererMeshOperations::CachedMeshFor(
		const Chunk& chunk)
	{
		const auto cached_mesh = std::ranges::find_if(cached_chunk_meshes_,
			[&chunk](const VulkanGpuCachedChunkMesh& candidate) noexcept
			{
				return candidate.chunk_x == chunk.GetChunkX() &&
					candidate.chunk_z == chunk.GetChunkZ();
			});
		if (cached_mesh != cached_chunk_meshes_.end()) return *cached_mesh;
		cached_chunk_meshes_.push_back(VulkanGpuCachedChunkMesh{
			chunk.GetChunkX(), chunk.GetChunkZ()
		});
		return cached_chunk_meshes_.back();
	}

	void VulkanGpuChunkRendererMeshOperations::AppendCachedChunkMesh(
		const VulkanGpuCachedChunkMesh& cached_mesh,
		std::vector<VoxelVertex>& vertices,
		std::vector<std::uint32_t>& indices) const
	{
		const std::uint32_t first_vertex = RenderElementCount(vertices.size());
		vertices.insert(vertices.end(), cached_mesh.vertices.begin(), cached_mesh.vertices.end());
		indices.reserve(indices.size() + cached_mesh.indices.size());
		for (std::uint32_t index : cached_mesh.indices)
		{
			indices.push_back(first_vertex + index);
		}
	}
}
