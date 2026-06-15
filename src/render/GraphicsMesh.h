#pragma once

#include "GraphicsTypes.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace ve::rendering
{
	enum class PrimitiveTopology
	{
		Triangles,
		Lines
	};

	template <typename VertexAllocator = std::allocator<Vertex3D>, typename IndexAllocator = std::allocator<std::uint32_t>>
	struct BasicIndexedMesh
	{
		using VertexList = std::vector<Vertex3D, VertexAllocator>;
		using IndexList = std::vector<std::uint32_t, IndexAllocator>;

		PrimitiveTopology topology = PrimitiveTopology::Triangles;
		VertexList vertices;
		IndexList indices;

		/** @return True when the mesh has no vertices to submit. */
		[[nodiscard]] bool Empty() const noexcept { return vertices.empty(); }

		/** @return Number of triangles represented by this mesh. */
		[[nodiscard]] std::size_t TriangleCount() const noexcept
		{
			const std::size_t element_count = indices.empty() ? vertices.size() : indices.size();
			return topology == PrimitiveTopology::Triangles ? element_count / 3U : 0U;
		}

		/** @return Axis-aligned bounds computed from every vertex position. */
		[[nodiscard]] GraphicsAabb Bounds() const noexcept
		{
			GraphicsAabb bounds = GraphicsAabb::Empty();
			for (const Vertex3D& vertex : vertices) bounds.Expand(vertex.position);
			return bounds;
		}
	};

	using IndexedMesh = BasicIndexedMesh<>;
}
