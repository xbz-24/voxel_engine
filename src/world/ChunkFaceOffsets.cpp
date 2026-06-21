#include "ChunkFaceOffsets.h"

namespace
{
	constexpr float BlockHalfSize = 0.5f;
}

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
		static constexpr FaceOffsets Top{{
			{-BlockHalfSize, BlockHalfSize, -BlockHalfSize, 0.0f, 0.0f}, {-BlockHalfSize, BlockHalfSize, BlockHalfSize, 0.0f, 1.0f},
			{BlockHalfSize, BlockHalfSize, BlockHalfSize, 1.0f, 1.0f}, {BlockHalfSize, BlockHalfSize, -BlockHalfSize, 1.0f, 0.0f}
		}};
		static constexpr FaceOffsets Bottom{{
			{-BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 0.0f, 0.0f}, {BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 1.0f, 0.0f},
			{BlockHalfSize, -BlockHalfSize, BlockHalfSize, 1.0f, 1.0f}, {-BlockHalfSize, -BlockHalfSize, BlockHalfSize, 0.0f, 1.0f}
		}};
		static constexpr FaceOffsets Front{{
			{-BlockHalfSize, -BlockHalfSize, BlockHalfSize, 0.0f, 0.0f}, {BlockHalfSize, -BlockHalfSize, BlockHalfSize, 1.0f, 0.0f},
			{BlockHalfSize, BlockHalfSize, BlockHalfSize, 1.0f, 1.0f}, {-BlockHalfSize, BlockHalfSize, BlockHalfSize, 0.0f, 1.0f}
		}};
		static constexpr FaceOffsets Back{{
			{-BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 1.0f, 0.0f}, {-BlockHalfSize, BlockHalfSize, -BlockHalfSize, 1.0f, 1.0f},
			{BlockHalfSize, BlockHalfSize, -BlockHalfSize, 0.0f, 1.0f}, {BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 0.0f, 0.0f}
		}};
		static constexpr FaceOffsets Right{{
			{BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 1.0f, 0.0f}, {BlockHalfSize, BlockHalfSize, -BlockHalfSize, 1.0f, 1.0f},
			{BlockHalfSize, BlockHalfSize, BlockHalfSize, 0.0f, 1.0f}, {BlockHalfSize, -BlockHalfSize, BlockHalfSize, 0.0f, 0.0f}
		}};
		static constexpr FaceOffsets Left{{
			{-BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 0.0f, 0.0f}, {-BlockHalfSize, -BlockHalfSize, BlockHalfSize, 1.0f, 0.0f},
			{-BlockHalfSize, BlockHalfSize, BlockHalfSize, 1.0f, 1.0f}, {-BlockHalfSize, BlockHalfSize, -BlockHalfSize, 0.0f, 1.0f}
		}};

		switch (direction)
		{
		case MeshFaceDirection::Top: return Top;
		case MeshFaceDirection::Bottom: return Bottom;
		case MeshFaceDirection::Front: return Front;
		case MeshFaceDirection::Back: return Back;
		case MeshFaceDirection::Right: return Right;
		case MeshFaceDirection::Left: return Left;
		}
		return Top;
	}
}
