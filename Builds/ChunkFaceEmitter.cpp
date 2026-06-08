#include "ChunkFaceEmitter.h"

#include <array>

namespace ve::world::mesh
{
	namespace
	{
		constexpr float BlockHalfSize = 0.5f;

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
		 * Writes four offsets as concrete render vertices.
		 *
		 * @param face Source face with world origin and tint.
		 * @param offsets Local offsets for the requested cube side.
		 * @param vertices Destination vertex buffer.
		 */
		void AppendOffsets(const MeshFace& face, const FaceOffsets& offsets, std::vector<ve::rendering::ChunkVertex>& vertices)
		{
			for (const FaceVertexOffset& offset : offsets)
			{
				vertices.push_back(ve::rendering::ChunkVertex{
					face.x + offset.x,
					face.y + offset.y,
					face.z + offset.z,
					offset.u,
					offset.v,
					face.r,
					face.g,
					face.b
				});
			}
		}
	}

	void AppendFaceVertices(const MeshFace& face, std::vector<ve::rendering::ChunkVertex>& vertices)
	{
		static constexpr FaceOffsets Top{{
			{-BlockHalfSize, BlockHalfSize, -BlockHalfSize, 0.0f, 0.0f},
			{-BlockHalfSize, BlockHalfSize, BlockHalfSize, 0.0f, 1.0f},
			{BlockHalfSize, BlockHalfSize, BlockHalfSize, 1.0f, 1.0f},
			{BlockHalfSize, BlockHalfSize, -BlockHalfSize, 1.0f, 0.0f}
		}};
		static constexpr FaceOffsets Bottom{{
			{-BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 0.0f, 0.0f},
			{BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 1.0f, 0.0f},
			{BlockHalfSize, -BlockHalfSize, BlockHalfSize, 1.0f, 1.0f},
			{-BlockHalfSize, -BlockHalfSize, BlockHalfSize, 0.0f, 1.0f}
		}};
		static constexpr FaceOffsets Front{{
			{-BlockHalfSize, -BlockHalfSize, BlockHalfSize, 0.0f, 0.0f},
			{BlockHalfSize, -BlockHalfSize, BlockHalfSize, 1.0f, 0.0f},
			{BlockHalfSize, BlockHalfSize, BlockHalfSize, 1.0f, 1.0f},
			{-BlockHalfSize, BlockHalfSize, BlockHalfSize, 0.0f, 1.0f}
		}};
		static constexpr FaceOffsets Back{{
			{-BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 1.0f, 0.0f},
			{-BlockHalfSize, BlockHalfSize, -BlockHalfSize, 1.0f, 1.0f},
			{BlockHalfSize, BlockHalfSize, -BlockHalfSize, 0.0f, 1.0f},
			{BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 0.0f, 0.0f}
		}};
		static constexpr FaceOffsets Right{{
			{BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 1.0f, 0.0f},
			{BlockHalfSize, BlockHalfSize, -BlockHalfSize, 1.0f, 1.0f},
			{BlockHalfSize, BlockHalfSize, BlockHalfSize, 0.0f, 1.0f},
			{BlockHalfSize, -BlockHalfSize, BlockHalfSize, 0.0f, 0.0f}
		}};
		static constexpr FaceOffsets Left{{
			{-BlockHalfSize, -BlockHalfSize, -BlockHalfSize, 0.0f, 0.0f},
			{-BlockHalfSize, -BlockHalfSize, BlockHalfSize, 1.0f, 0.0f},
			{-BlockHalfSize, BlockHalfSize, BlockHalfSize, 1.0f, 1.0f},
			{-BlockHalfSize, BlockHalfSize, -BlockHalfSize, 0.0f, 1.0f}
		}};

		switch (face.direction)
		{
		case MeshFaceDirection::Top: AppendOffsets(face, Top, vertices); break;
		case MeshFaceDirection::Bottom: AppendOffsets(face, Bottom, vertices); break;
		case MeshFaceDirection::Front: AppendOffsets(face, Front, vertices); break;
		case MeshFaceDirection::Back: AppendOffsets(face, Back, vertices); break;
		case MeshFaceDirection::Right: AppendOffsets(face, Right, vertices); break;
		case MeshFaceDirection::Left: AppendOffsets(face, Left, vertices); break;
		}
	}
}
