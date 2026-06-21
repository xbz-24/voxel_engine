#pragma once

#include "ChunkMeshTypes.h"

#include <array>

namespace ve::world::mesh
{
	struct FaceVertexOffset
	{
		float x;
		float y;
		float z;
		float u;
		float v;
	};

	using FaceOffsets = std::array<FaceVertexOffset, 4>;

	/**
	 * Selects the local vertex offsets for one voxel face direction.
	 *
	 * @param direction Cube side that needs vertex offsets.
	 * @return Four local offsets ordered for GL_QUADS rendering.
	 */
	const FaceOffsets& OffsetsForDirection(MeshFaceDirection direction);
}
