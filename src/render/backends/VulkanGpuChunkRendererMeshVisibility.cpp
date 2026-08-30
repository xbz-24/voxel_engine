#include "VulkanGpuChunkRenderer.h"

#include "World.h"

namespace ve::rendering
{
	void VulkanGpuChunkRendererMeshOperations::AppendVisibleBlockFaces(
		const ve::world::World& world,
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
			const ve::blocks::BlockId neighbor = world.GetBlock(
				block_x + face.neighbor_offset.x,
				block_y + face.neighbor_offset.y,
				block_z + face.neighbor_offset.z);
			if (!OccludesNeighborFaces(block_registry, neighbor))
			{
				AppendFaceMesh(face, world, block_registry,
					block_x, block_y, block_z, block, vertices, indices);
			}
		}
	}
}
