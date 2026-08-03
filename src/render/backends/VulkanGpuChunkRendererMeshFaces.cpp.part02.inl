			const glm::vec3 vertex_position = block_origin + ChunkFaceCorner(face, corner);
			const float vertex_light = std::clamp(
				face.light * BlockLightBoost(block) * CornerOcclusion(world, block_registry, face, block_coordinate, corner),
				0.20f,
				1.70f);
			vertices.push_back(VoxelVertex{
				vertex_position,
				PackVoxelColor(color.r, color.g, color.b, color.a),
				vertex_light,
				PackVoxelNormal(face.neighbor_offset)
			});
		}
		AppendQuadIndices(indices, first_face_vertex_index);
	}
}
