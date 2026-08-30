#pragma once

#include "Block.h"
#include "VulkanGpuChunkGeometry.h"
#include "VulkanGpuChunkRendererResourceOperations.h"

#include <span>

class Chunk;

namespace ve::world
{
	class World;
}

namespace ve::rendering
{
	class VulkanGpuChunkRendererMeshOperations :
		protected VulkanGpuChunkRendererResourceOperations
	{
	protected:
		[[nodiscard]] bool UploadMeshBuffers(
			std::span<const VoxelVertex> vertices,
			std::span<const std::uint32_t> indices,
			std::span<const std::uint32_t> shadow_indices);
		void ReleaseMeshBuffers();
		void RebuildMesh(
			const ve::world::World& world,
			const ve::blocks::BlockRegistry& registry,
			std::vector<VoxelVertex>& vertices,
			std::vector<std::uint32_t>& indices);
		void ResetChunkMeshCacheForWorldStorage(const ve::world::World& world);
		[[nodiscard]] VulkanGpuCachedChunkMesh& CachedMeshFor(const Chunk& chunk);
		void RebuildChunkMesh(
			const ve::world::World& world,
			const ve::blocks::BlockRegistry& registry,
			const Chunk& chunk,
			VulkanGpuCachedChunkMesh& cached_mesh) const;
		void AppendCachedChunkMesh(
			const VulkanGpuCachedChunkMesh& cached_mesh,
			std::vector<VoxelVertex>& vertices,
			std::vector<std::uint32_t>& indices) const;
		void AppendVisibleBlockFaces(
			const ve::world::World& world,
			const ve::blocks::BlockRegistry& registry,
			int block_x, int block_y, int block_z,
			ve::blocks::BlockId block,
			std::vector<VoxelVertex>& vertices,
			std::vector<std::uint32_t>& indices) const;
		void AppendFaceMesh(
			const BlockFaceGeometry& face,
			const ve::world::World& world,
			const ve::blocks::BlockRegistry& registry,
			int block_x, int block_y, int block_z,
			ve::blocks::BlockId block,
			std::vector<VoxelVertex>& vertices,
			std::vector<std::uint32_t>& indices) const;
	};
}
