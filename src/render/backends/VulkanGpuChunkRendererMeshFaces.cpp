#include "VulkanGpuChunkRenderer.h"

#include "BlockSolidColor.h"
#include "VulkanGpuChunkRendererMeshLighting.h"

#include <algorithm>

namespace ve::rendering
{
	void VulkanGpuChunkRendererMeshOperations::AppendFaceMesh(
		const BlockFaceGeometry& face,
		const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		int block_x,
		int block_y,
		int block_z,
		ve::blocks::BlockId block,
		std::vector<VoxelVertex>& vertices,
		std::vector<std::uint32_t>& indices) const
	{
		const std::uint32_t first_vertex = RenderElementCount(vertices.size());
		const ve::blocks::SolidBlockColor color =
			ve::blocks::JitteredSolidColor(block, block_x, block_y, block_z);
		const glm::ivec3 block_coordinate{ block_x, block_y, block_z };
		const glm::vec3 block_origin{
			VertexCoordinate(block_x),
			VertexCoordinate(block_y),
			VertexCoordinate(block_z)
		};
		for (std::size_t corner = 0; corner < face.offsets.size(); ++corner)
		{
			const float vertex_light = std::clamp(
				face.light * detail::BlockLightBoost(block) *
					detail::CornerOcclusion(
						world, block_registry, face, block_coordinate, corner),
				0.20f,
				1.70f);
			vertices.push_back(VoxelVertex{
				block_origin + ChunkFaceCorner(face, corner),
				PackVoxelColor(color.r, color.g, color.b, color.a),
				vertex_light,
				PackVoxelNormal(face.neighbor_offset)
			});
		}
		AppendQuadIndices(indices, first_vertex);
	}
}
