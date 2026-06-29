		void PlaceImportedVoxel(ve::world::World& world,
			const DemoBounds& bounds,
			std::unordered_set<std::uint64_t>& occupied,
			const glm::vec3& position,
			BlockId block,
			std::size_t& budget)
		{
			if (budget == 0) return;
			const int block_x = static_cast<int>(std::round(position.x));
			const int block_y = static_cast<int>(std::round(position.y));
			const int block_z = static_cast<int>(std::round(position.z));
			if (!IsInside(bounds, block_x, block_y, block_z)) return;
			const std::uint64_t voxel_key = VoxelKey(block_x, block_y, block_z);
			if (!occupied.insert(voxel_key).second) return;
			SetBlock(world, bounds, block_x, block_y, block_z, block);
			--budget;
		}

		void VoxelizeTriangle(ve::world::World& world,
			const DemoBounds& bounds,
			std::unordered_set<std::uint64_t>& occupied,
			glm::vec3 first_vertex,
			glm::vec3 second_vertex,
			glm::vec3 third_vertex,
			BlockId block,
			std::size_t& budget)
		{
			const float longest_edge = std::max({
				glm::length(first_vertex - second_vertex),
				glm::length(second_vertex - third_vertex),
				glm::length(third_vertex - first_vertex)
			});
			const int sample_count = std::clamp(static_cast<int>(std::ceil(longest_edge * 1.55f)), 1, 24);
			for (int barycentric_u_step = 0; barycentric_u_step <= sample_count && budget > 0; ++barycentric_u_step)
			{
				for (int barycentric_v_step = 0; barycentric_v_step <= sample_count - barycentric_u_step && budget > 0; ++barycentric_v_step)
				{
					const float barycentric_u = static_cast<float>(barycentric_u_step) / static_cast<float>(sample_count);
					const float barycentric_v = static_cast<float>(barycentric_v_step) / static_cast<float>(sample_count);
					const float barycentric_w = 1.0f - barycentric_u - barycentric_v;
					const glm::vec3 sampled_position =
						(first_vertex * barycentric_u) +
						(second_vertex * barycentric_v) +
						(third_vertex * barycentric_w);
					PlaceImportedVoxel(world, bounds, occupied, sampled_position, block, budget);
				}
			}
		}

		[[nodiscard]] bool VoxelizeImportedModel(ve::world::World& world,
			const DemoBounds& bounds,
			const ve::assets::ImportedModel& model,
			int base_y,
			int target_extent,
			std::size_t voxel_budget)
		{
			glm::vec3 minimum{};
			glm::vec3 maximum{};
			if (!ModelBounds(model, minimum, maximum)) return false;
			const glm::vec3 source_size = maximum - minimum;
			const float longest_axis = std::max({ source_size.x, source_size.y, source_size.z });
			if (longest_axis <= 0.0001f) return false;
			const float scale = static_cast<float>(target_extent) / longest_axis;
			const glm::vec3 target_size = source_size * scale;
			const glm::vec3 target_origin{
				static_cast<float>(bounds.center_x) - (target_size.x * 0.5f),
				static_cast<float>(base_y),
				static_cast<float>(bounds.center_z) - (target_size.z * 0.5f)
			};

			std::unordered_set<std::uint64_t> occupied;
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
