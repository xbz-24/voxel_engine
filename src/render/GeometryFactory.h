#pragma once

#include "GraphicsMesh.h"

namespace ve::rendering
{
	/** Factory object for common procedural meshes used by demos and tools. */
	class GeometryFactory
	{
	public:
		/** @param first First point. @param second Second point. @param third Third point. @param color Vertex color. @return Triangle mesh. */
		[[nodiscard]] static IndexedMesh Triangle(glm::vec3 first, glm::vec3 second, glm::vec3 third, ColorRgba color = {});

		/** @param rect Centered rectangle size. @param color Vertex color. @return Quad mesh on the XY plane. */
		[[nodiscard]] static IndexedMesh Quad(glm::vec2 rect, ColorRgba color = {});

		/** @param center Cube center. @param size Cube size by axis. @param color Vertex color. @return Cube mesh. */
		[[nodiscard]] static IndexedMesh Cube(glm::vec3 center, glm::vec3 size, ColorRgba color = {});
	};
}
