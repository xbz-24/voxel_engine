#include "ChunkFaceEmitter.h"

#include "ChunkFaceOffsets.h"

namespace ve::world::mesh
{
	namespace
	{
		/**
		 * Scales one unit-face offset by a greedy merged face size.
		 *
		 * @param face Source face containing width and height.
		 * @param offset Unit face offset to scale.
		 * @return Offset expanded to the merged rectangle dimensions.
		 */
		FaceVertexOffset ScaleOffset(const MeshFace& face, FaceVertexOffset offset)
		{
			const float width = static_cast<float>(face.width);
			const float height = static_cast<float>(face.height);
			if (face.direction == MeshFaceDirection::Top || face.direction == MeshFaceDirection::Bottom)
			{
				offset.x *= width;
				offset.z *= height;
			}
			else if (face.direction == MeshFaceDirection::Front || face.direction == MeshFaceDirection::Back)
			{
				offset.x *= width;
				offset.y *= height;
			}
			else
			{
				offset.z *= width;
				offset.y *= height;
			}
			offset.u *= width;
			offset.v *= height;
			return offset;
		}

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
				const FaceVertexOffset scaledOffset = ScaleOffset(face, offset);
				vertices.push_back(ve::rendering::ChunkVertex{
					face.x + scaledOffset.x,
					face.y + scaledOffset.y,
					face.z + scaledOffset.z,
					scaledOffset.u,
					scaledOffset.v,
					face.r,
					face.g,
					face.b
				});
			}
		}
	}

	/**
	 * Expands one collected cube face into four textured chunk vertices.
	 *
	 * @param face Face to expand.
	 * @param vertices Destination vertex buffer.
	 */
	void AppendFaceVertices(const MeshFace& face, std::vector<ve::rendering::ChunkVertex>& vertices)
	{
		AppendOffsets(face, OffsetsForDirection(face.direction), vertices);
	}
}
