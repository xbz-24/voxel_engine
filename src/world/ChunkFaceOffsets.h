#pragma once

#include "ChunkMeshTypes.h"

#include <array>

namespace ve::world::mesh
{
	struct FaceVertexOffset
	{
		float local_position_x;
		float local_position_y;
		float local_position_z;
		float texture_u;
		float texture_v;
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
