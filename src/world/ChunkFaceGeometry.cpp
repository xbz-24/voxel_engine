#include "ChunkFaceGeometry.h"

#include <array>

namespace
{
	constexpr float kBlockHalfSize = 0.5f;
}

namespace ve::world::mesh
{
	std::span<const ChunkFaceGeometry> ChunkFaceGeometries() noexcept
	{
		static const std::array<ChunkFaceGeometry, 6> kFaces{
			ChunkFaceGeometry{
				ve::blocks::BlockFace::Top,
				MeshFaceDirection::Top,
				{ 0, 1, 0 },
				FaceOffsets{{
					{ -kBlockHalfSize, kBlockHalfSize, -kBlockHalfSize, 0.0f, 0.0f },
					{ -kBlockHalfSize, kBlockHalfSize, kBlockHalfSize, 0.0f, 1.0f },
					{ kBlockHalfSize, kBlockHalfSize, kBlockHalfSize, 1.0f, 1.0f },
					{ kBlockHalfSize, kBlockHalfSize, -kBlockHalfSize, 1.0f, 0.0f }
				}},
				1.22f
			},
			ChunkFaceGeometry{
				ve::blocks::BlockFace::Bottom,
				MeshFaceDirection::Bottom,
				{ 0, -1, 0 },
				FaceOffsets{{
					{ -kBlockHalfSize, -kBlockHalfSize, -kBlockHalfSize, 0.0f, 0.0f },
					{ kBlockHalfSize, -kBlockHalfSize, -kBlockHalfSize, 1.0f, 0.0f },
					{ kBlockHalfSize, -kBlockHalfSize, kBlockHalfSize, 1.0f, 1.0f },
					{ -kBlockHalfSize, -kBlockHalfSize, kBlockHalfSize, 0.0f, 1.0f }
				}},
				0.24f
			},
			ChunkFaceGeometry{
				ve::blocks::BlockFace::Front,
				MeshFaceDirection::Front,
				{ 0, 0, 1 },
				FaceOffsets{{
					{ -kBlockHalfSize, -kBlockHalfSize, kBlockHalfSize, 0.0f, 0.0f },
					{ kBlockHalfSize, -kBlockHalfSize, kBlockHalfSize, 1.0f, 0.0f },
					{ kBlockHalfSize, kBlockHalfSize, kBlockHalfSize, 1.0f, 1.0f },
					{ -kBlockHalfSize, kBlockHalfSize, kBlockHalfSize, 0.0f, 1.0f }
				}},
				0.90f
			},
			ChunkFaceGeometry{
				ve::blocks::BlockFace::Back,
				MeshFaceDirection::Back,
				{ 0, 0, -1 },
				FaceOffsets{{
					{ -kBlockHalfSize, -kBlockHalfSize, -kBlockHalfSize, 1.0f, 0.0f },
					{ -kBlockHalfSize, kBlockHalfSize, -kBlockHalfSize, 1.0f, 1.0f },
					{ kBlockHalfSize, kBlockHalfSize, -kBlockHalfSize, 0.0f, 1.0f },
					{ kBlockHalfSize, -kBlockHalfSize, -kBlockHalfSize, 0.0f, 0.0f }
				}},
				0.44f
			},
			ChunkFaceGeometry{
				ve::blocks::BlockFace::Right,
				MeshFaceDirection::Right,
				{ 1, 0, 0 },
				FaceOffsets{{
					{ kBlockHalfSize, -kBlockHalfSize, -kBlockHalfSize, 1.0f, 0.0f },
					{ kBlockHalfSize, kBlockHalfSize, -kBlockHalfSize, 1.0f, 1.0f },
					{ kBlockHalfSize, kBlockHalfSize, kBlockHalfSize, 0.0f, 1.0f },
					{ kBlockHalfSize, -kBlockHalfSize, kBlockHalfSize, 0.0f, 0.0f }
				}},
				1.02f
			},
			ChunkFaceGeometry{
				ve::blocks::BlockFace::Left,
				MeshFaceDirection::Left,
				{ -1, 0, 0 },
				FaceOffsets{{
					{ -kBlockHalfSize, -kBlockHalfSize, -kBlockHalfSize, 0.0f, 0.0f },
					{ -kBlockHalfSize, -kBlockHalfSize, kBlockHalfSize, 1.0f, 0.0f },
					{ -kBlockHalfSize, kBlockHalfSize, kBlockHalfSize, 1.0f, 1.0f },
					{ -kBlockHalfSize, kBlockHalfSize, -kBlockHalfSize, 0.0f, 1.0f }
				}},
				0.58f
			}
		};
		return kFaces;
	}

	const ChunkFaceGeometry& GeometryForDirection(MeshFaceDirection direction) noexcept
	{
		for (const ChunkFaceGeometry& geometry : ChunkFaceGeometries())
		{
			if (geometry.direction == direction)
			{
				return geometry;
			}
		}
		return ChunkFaceGeometries().front();
	}
}
