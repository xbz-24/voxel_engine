#include "ChunkFaceOffsets.h"

#include "ChunkFaceGeometry.h"

namespace ve::world::mesh
{
	/**
	 * Selects the local vertex offsets for one voxel face direction.
	 *
	 * @param direction Cube side that needs vertex offsets.
	 * @return Four local offsets ordered for GL_QUADS rendering.
	 */
	const FaceOffsets& OffsetsForDirection(MeshFaceDirection direction)
	{
		return GeometryForDirection(direction).offsets;
	}
}
