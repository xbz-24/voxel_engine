		void PlaceImportedVoxel(ve::world::World& world,
			const DemoBounds& bounds,
			std::unordered_set<std::uint64_t>& occupied,
			const glm::vec3& position,
			BlockId block,
			std::size_t& budget)
		{
			if (budget == 0) return;
			const int x = static_cast<int>(std::round(position.x));
			const int y = static_cast<int>(std::round(position.y));
			const int z = static_cast<int>(std::round(position.z));
			if (!IsInside(bounds, x, y, z)) return;
			const std::uint64_t key = VoxelKey(x, y, z);
			if (!occupied.insert(key).second) return;
			SetBlock(world, bounds, x, y, z, block);
			--budget;
		}

		void VoxelizeTriangle(ve::world::World& world,
			const DemoBounds& bounds,
			std::unordered_set<std::uint64_t>& occupied,
			glm::vec3 a,
			glm::vec3 b,
			glm::vec3 c,
			BlockId block,
			std::size_t& budget)
		{
			const float longest = std::max({ glm::length(a - b), glm::length(b - c), glm::length(c - a) });
			const int samples = std::clamp(static_cast<int>(std::ceil(longest * 1.55f)), 1, 24);
			for (int u = 0; u <= samples && budget > 0; ++u)
			{
				for (int v = 0; v <= samples - u && budget > 0; ++v)
				{
					const float fu = static_cast<float>(u) / static_cast<float>(samples);
					const float fv = static_cast<float>(v) / static_cast<float>(samples);
					const float fw = 1.0f - fu - fv;
					PlaceImportedVoxel(world, bounds, occupied, (a * fu) + (b * fv) + (c * fw), block, budget);
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
					const std::uint32_t ai = mesh.indices[index];
					const std::uint32_t bi = mesh.indices[index + 1u];
					const std::uint32_t ci = mesh.indices[index + 2u];
					if (ai >= mesh.vertices.size() || bi >= mesh.vertices.size() || ci >= mesh.vertices.size()) continue;
					const glm::vec3 source_centroid = (mesh.vertices[ai].position + mesh.vertices[bi].position + mesh.vertices[ci].position) / 3.0f;
					const glm::vec2 source_uv = (mesh.vertices[ai].texture_coordinates + mesh.vertices[bi].texture_coordinates + mesh.vertices[ci].texture_coordinates) / 3.0f;
					const std::optional<ve::blocks::SolidBlockColor> sampled_color = SampleMaterialColor(model, mesh, source_uv, texture_cache);
					const BlockId block = BlockForImportedTriangle(model, mesh, source_centroid, minimum, maximum, sampled_color);
					const glm::vec3 a = ModelToVoxelPosition(mesh.vertices[ai].position, minimum, target_origin, scale);
					const glm::vec3 b = ModelToVoxelPosition(mesh.vertices[bi].position, minimum, target_origin, scale);
					const glm::vec3 c = ModelToVoxelPosition(mesh.vertices[ci].position, minimum, target_origin, scale);
					VoxelizeTriangle(world, bounds, occupied, a, b, c, block, voxel_budget);
				}
			}
			return !occupied.empty();
		}
