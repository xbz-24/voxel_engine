#include "VulkanChunkMeshTranslator.h"

namespace ve::rendering
{
	VulkanChunkMeshPayload BuildVulkanChunkMeshPayload(const ve::world::mesh::ChunkMeshBuildResult& mesh)
	{
		VulkanChunkMeshPayload payload{};
		payload.vertices = mesh.vertices;
		payload.indices.reserve((mesh.vertices.size() / 4u) * 6u);
		for (std::uint32_t base_vertex = 0; base_vertex + 3u < mesh.vertices.size(); base_vertex += 4u)
		{
			payload.indices.push_back(base_vertex);
			payload.indices.push_back(base_vertex + 1u);
			payload.indices.push_back(base_vertex + 2u);
			payload.indices.push_back(base_vertex);
			payload.indices.push_back(base_vertex + 2u);
			payload.indices.push_back(base_vertex + 3u);
		}
		payload.draw.index_count = static_cast<std::uint32_t>(payload.indices.size());
		payload.draw.instance_count = payload.draw.index_count > 0u ? 1u : 0u;
		return payload;
	}
}
