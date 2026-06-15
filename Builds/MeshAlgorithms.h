#pragma once

#include "GraphicsMesh.h"

#include <glm/geometric.hpp>
#include <utility>

namespace ve::rendering
{
	/**
	 * Recalculates area-weighted smooth vertex normals for an indexed triangle mesh.
	 *
	 * @tparam VertexAllocator Allocator used by vertex storage.
	 * @tparam IndexAllocator Allocator used by index storage.
	 * @param mesh Mesh whose vertex normals should be replaced.
	 */
	template <typename VertexAllocator, typename IndexAllocator>
	void RecalculateSmoothNormals(BasicIndexedMesh<VertexAllocator, IndexAllocator>& mesh)
	{
		for (Vertex3D& vertex : mesh.vertices) vertex.normal = glm::vec3{ 0.0f };
		for (std::size_t index = 0; index + 2U < mesh.indices.size(); index += 3U)
		{
			const std::uint32_t first_index = mesh.indices[index];
			const std::uint32_t second_index = mesh.indices[index + 1U];
			const std::uint32_t third_index = mesh.indices[index + 2U];
			if (first_index >= mesh.vertices.size() || second_index >= mesh.vertices.size() ||
				third_index >= mesh.vertices.size()) continue;
			const glm::vec3 first_edge = mesh.vertices[second_index].position - mesh.vertices[first_index].position;
			const glm::vec3 second_edge = mesh.vertices[third_index].position - mesh.vertices[first_index].position;
			const glm::vec3 area_weighted_normal = glm::cross(first_edge, second_edge);
			mesh.vertices[first_index].normal += area_weighted_normal;
			mesh.vertices[second_index].normal += area_weighted_normal;
			mesh.vertices[third_index].normal += area_weighted_normal;
		}
		for (Vertex3D& vertex : mesh.vertices)
		{
			if (glm::length(vertex.normal) > 0.0f) vertex.normal = glm::normalize(vertex.normal);
		}
	}

	/**
	 * Removes indexed triangles whose area is below a selected epsilon.
	 *
	 * @tparam VertexAllocator Allocator used by vertex storage.
	 * @tparam IndexAllocator Allocator used by index storage.
	 * @param mesh Mesh whose index buffer should be compacted.
	 * @param minimum_double_area Minimum cross-product length accepted as geometry.
	 * @return Number of triangles removed from the index buffer.
	 */
	template <typename VertexAllocator, typename IndexAllocator>
	[[nodiscard]] std::size_t RemoveDegenerateTriangles(
		BasicIndexedMesh<VertexAllocator, IndexAllocator>& mesh, float minimum_double_area = 0.000001f)
	{
		typename BasicIndexedMesh<VertexAllocator, IndexAllocator>::IndexList compacted{ mesh.indices.get_allocator() };
		compacted.reserve(mesh.indices.size());
		std::size_t removed_triangle_count = 0;
		for (std::size_t index = 0; index + 2U < mesh.indices.size(); index += 3U)
		{
			const std::uint32_t first = mesh.indices[index];
			const std::uint32_t second = mesh.indices[index + 1U];
			const std::uint32_t third = mesh.indices[index + 2U];
			if (first >= mesh.vertices.size() || second >= mesh.vertices.size() || third >= mesh.vertices.size())
			{
				removed_triangle_count++;
				continue;
			}
			const glm::vec3 first_edge = mesh.vertices[second].position - mesh.vertices[first].position;
			const glm::vec3 second_edge = mesh.vertices[third].position - mesh.vertices[first].position;
			if (glm::length(glm::cross(first_edge, second_edge)) <= minimum_double_area)
			{
				removed_triangle_count++;
				continue;
			}
			compacted.insert(compacted.end(), { first, second, third });
		}
		mesh.indices = std::move(compacted);
		return removed_triangle_count;
	}
}
