			occupied.reserve(std::min<std::size_t>(voxel_budget, 300'000u));
			TextureCache texture_cache;
			for (const ve::assets::ImportedMesh& mesh : model.meshes)
			{
				for (std::size_t index = 0; index + 2 < mesh.indices.size() && voxel_budget > 0; index += 3)
				{
					const std::uint32_t first_vertex_index = mesh.indices[index];
					const std::uint32_t second_vertex_index = mesh.indices[index + 1u];
					const std::uint32_t third_vertex_index = mesh.indices[index + 2u];
					if (first_vertex_index >= mesh.vertices.size() ||
						second_vertex_index >= mesh.vertices.size() ||
						third_vertex_index >= mesh.vertices.size()) continue;

					const ve::assets::ImportedVertex& first_source_vertex = mesh.vertices[first_vertex_index];
					const ve::assets::ImportedVertex& second_source_vertex = mesh.vertices[second_vertex_index];
					const ve::assets::ImportedVertex& third_source_vertex = mesh.vertices[third_vertex_index];
					const glm::vec3 source_centroid =
						(first_source_vertex.position + second_source_vertex.position + third_source_vertex.position) / 3.0f;
					const glm::vec2 source_uv =
						(first_source_vertex.texture_coordinates +
							second_source_vertex.texture_coordinates +
							third_source_vertex.texture_coordinates) / 3.0f;
					const std::optional<ve::blocks::SolidBlockColor> sampled_color = SampleMaterialColor(model, mesh, source_uv, texture_cache);
					const BlockId block = BlockForImportedTriangle(model, mesh, source_centroid, minimum, maximum, sampled_color);
					const glm::vec3 first_voxel_vertex =
						ModelToVoxelPosition(first_source_vertex.position, minimum, target_origin, scale);
					const glm::vec3 second_voxel_vertex =
						ModelToVoxelPosition(second_source_vertex.position, minimum, target_origin, scale);
					const glm::vec3 third_voxel_vertex =
						ModelToVoxelPosition(third_source_vertex.position, minimum, target_origin, scale);
					VoxelizeTriangle(
						world,
						bounds,
						occupied,
						first_voxel_vertex,
						second_voxel_vertex,
						third_voxel_vertex,
						block,
						voxel_budget);
				}
			}
			return !occupied.empty();
		}
