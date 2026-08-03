		void PlaceImportedVoxel(ve::world::World& world,
			const DemoBounds& bounds,
			std::unordered_set<std::uint64_t>& occupied,
			const glm::vec3& position,
			BlockId block,
			std::size_t& budget)
		{
			if (budget == 0) return;
			const int block_x = RoundedSceneInt(position.x);
			const int block_y = RoundedSceneInt(position.y);
			const int block_z = RoundedSceneInt(position.z);
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
			const int sample_count = std::clamp(CeilSceneInt(longest_edge * 1.55f), 1, 24);
			for (int barycentric_u_step = 0; barycentric_u_step <= sample_count && budget > 0; ++barycentric_u_step)
			{
				for (int barycentric_v_step = 0; barycentric_v_step <= sample_count - barycentric_u_step && budget > 0; ++barycentric_v_step)
				{
					const float barycentric_u = NormalizedRatio(barycentric_u_step, sample_count);
					const float barycentric_v = NormalizedRatio(barycentric_v_step, sample_count);
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
			const float scale = SceneFloat(target_extent) / longest_axis;
			const glm::vec3 target_size = source_size * scale;
			const glm::vec3 target_origin{
				SceneFloat(bounds.center_x) - (target_size.x * 0.5f),
				SceneFloat(base_y),
				SceneFloat(bounds.center_z) - (target_size.z * 0.5f)
			};

			std::unordered_set<std::uint64_t> occupied;
