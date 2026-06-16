#include "VulkanChunkMeshTranslator.h"

namespace ve::rendering
{
	namespace
	{
		constexpr std::uint32_t kQuadVertexCount = 4u;
		constexpr std::uint32_t kQuadIndexCount = 6u;
	}

	VulkanChunkMeshPayload BuildVulkanChunkMeshPayload(const ve::world::mesh::ChunkMeshBuildResult& mesh)
	{
		VulkanChunkMeshPayload payload{};
		payload.vertices = mesh.vertices;
		payload.indices.reserve((mesh.vertices.size() / kQuadVertexCount) * kQuadIndexCount);
		for (std::uint32_t base_vertex = 0; base_vertex + 3u < mesh.vertices.size(); base_vertex += kQuadVertexCount)
		{
			payload.indices.push_back(base_vertex);
			payload.indices.push_back(base_vertex + 1u);
			payload.indices.push_back(base_vertex + 2u);
			payload.indices.push_back(base_vertex);
			payload.indices.push_back(base_vertex + 2u);
			payload.indices.push_back(base_vertex + 3u);
		}
		payload.batches.reserve(mesh.batches.size());
		for (const ChunkMeshBatch& batch : mesh.batches)
		{
			const std::uint32_t first_quad = batch.first_vertex / kQuadVertexCount;
			const std::uint32_t quad_count = batch.vertex_count / kQuadVertexCount;
			const std::uint32_t index_count = quad_count * kQuadIndexCount;
			if (index_count > 0u)
			{
				payload.batches.push_back(VulkanChunkMeshBatch{
					batch.texture,
					first_quad * kQuadIndexCount,
					index_count
				});
			}
		}
		payload.draw.index_count = static_cast<std::uint32_t>(payload.indices.size());
		payload.draw.instance_count = payload.draw.index_count > 0u ? 1u : 0u;
		return payload;
	}
}
