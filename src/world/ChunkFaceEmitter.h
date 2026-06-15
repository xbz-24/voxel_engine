#pragma once

#include "ChunkMeshTypes.h"

#include <vector>

namespace ve::world::mesh
{
	/**
	 * Expands one collected cube face into four textured chunk vertices.
	 *
	 * @param face Face to expand.
	 * @param vertices Destination vertex buffer.
	 */
	void AppendFaceVertices(const MeshFace& face, std::vector<ve::rendering::ChunkVertex>& vertices);
}
