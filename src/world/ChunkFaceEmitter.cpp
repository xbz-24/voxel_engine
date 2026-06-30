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
				offset.local_position_x *= width;
				offset.local_position_z *= height;
			}
			else if (face.direction == MeshFaceDirection::Front || face.direction == MeshFaceDirection::Back)
			{
				offset.local_position_x *= width;
				offset.local_position_y *= height;
			}
			else
			{
				offset.local_position_z *= width;
				offset.local_position_y *= height;
			}
			offset.texture_u *= width;
			offset.texture_v *= height;
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
					face.world_center_x + scaledOffset.local_position_x,
					face.world_center_y + scaledOffset.local_position_y,
					face.world_center_z + scaledOffset.local_position_z,
					scaledOffset.texture_u,
					scaledOffset.texture_v,
					face.red,
					face.green,
					face.blue
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
