#include "VulkanGpuChunkRendererShadowIndices.h"

#include "VulkanGpuChunkRendererPacking.h"
#include "VulkanGpuChunkRendererShadowMath.h"

namespace ve::rendering::detail
{
	std::vector<std::uint32_t> BuildDirectionalShadowIndices(
		std::span<const VoxelVertex> vertices,
		std::span<const std::uint32_t> indices,
		glm::vec3 sun_direction)
	{
		std::vector<std::uint32_t> shadow_indices;
		shadow_indices.reserve(indices.size() / 2u);
		sun_direction = NormalizeVulkanShaderDirection(
			sun_direction, { -0.42f, 0.78f, -0.46f });
		for (std::size_t offset = 0; offset + 2u < indices.size(); offset += 3u)
		{
			const std::uint32_t first_vertex_index = indices[offset];
			if (first_vertex_index >= vertices.size()) continue;
			const glm::vec3 face_normal =
				UnpackVoxelNormal(vertices[first_vertex_index].normal_snorm8);
			if (glm::dot(face_normal, sun_direction) <= 0.001f) continue;
			shadow_indices.push_back(indices[offset]);
			shadow_indices.push_back(indices[offset + 1u]);
			shadow_indices.push_back(indices[offset + 2u]);
		}
		return shadow_indices;
	}
}
