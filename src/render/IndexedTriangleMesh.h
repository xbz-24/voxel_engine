#pragma once

#include "GraphicsGeometryTypes.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <span>

namespace ve::rendering
{
	/** Immutable indexed-triangle payload consumed synchronously by a backend upload. */
	struct IndexedTriangleMeshDescription
	{
		std::span<const Vertex3D> vertices;
		std::span<const std::uint32_t> indices;

		[[nodiscard]] bool IsValid() const noexcept;
	};

	/** Backend-owned indexed triangle mesh with an explicit native-resource lifetime. */
	class IndexedTriangleMesh
	{
	public:
		virtual ~IndexedTriangleMesh() = default;

		/** Replaces the current GPU payload. Invalid descriptions leave the resource empty. */
		[[nodiscard]] virtual bool Upload(
			const IndexedTriangleMeshDescription& description) = 0;

		/** Draws the uploaded triangles with the supplied object-to-world transform. */
		virtual void Draw(const glm::mat4& model_matrix) const = 0;

		/** Releases native objects; repeated calls are safe. */
		virtual void Release() noexcept = 0;
	};
}
