		void PlaceBoulder(ve::world::World& world, const DemoBounds& bounds, int cx, int base_y, int cz, int radius, int height, int seed)
		{
			const std::array<BlockId, 5> palette{ BlockId::Granite, BlockId::Andesite, BlockId::MossyCobblestone, BlockId::Stone, BlockId::MossBlock };
			for (int dx = -radius; dx <= radius; ++dx)
			{
				for (int dz = -radius; dz <= radius; ++dz)
				{
					for (int dy = 0; dy <= height; ++dy)
					{
						const float nx = static_cast<float>(dx) / static_cast<float>(std::max(radius, 1));
						const float nz = static_cast<float>(dz) / static_cast<float>(std::max(radius, 1));
						const float ny = static_cast<float>(dy) / static_cast<float>(std::max(height, 1));
						if ((nx * nx) + (nz * nz) + (ny * ny * 1.3f) > 1.05f) continue;
						if (Hash01(cx + dx, cz + dz + dy, seed) < 0.16f) continue;
						const std::size_t palette_index = static_cast<std::size_t>((std::abs(dx) + std::abs(dz) + dy + seed) % static_cast<int>(palette.size()));
						SetBlock(world, bounds, cx + dx, base_y + dy + 1, cz + dz, palette[palette_index]);
					}
				}
			}
		}

		SurfacePoint RandomSurfacePoint(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config, std::mt19937& rng)
		{
			const int min_x = std::clamp(bounds.center_x - config.terrain_radius + 6, 3, std::max(3, bounds.width - 4));
			const int max_x = std::clamp(bounds.center_x + config.terrain_radius - 6, min_x, std::max(min_x, bounds.width - 4));
			const int min_z = std::clamp(bounds.center_z - config.terrain_radius + 6, 3, std::max(3, bounds.depth - 4));
			const int max_z = std::clamp(bounds.center_z + config.terrain_radius - 6, min_z, std::max(min_z, bounds.depth - 4));
			std::uniform_int_distribution<int> x_dist(min_x, max_x);
			std::uniform_int_distribution<int> z_dist(min_z, max_z);
			const int x = x_dist(rng);
			const int z = z_dist(rng);
			return SurfacePoint{ x, FindGroundY(world, bounds, x, z, config.ground_y), z };
		}

		void BuildRockFields(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			std::mt19937 rng(static_cast<std::uint32_t>(config.seed + 501));
			for (int index = 0; index < config.house_count; ++index)
			{
				const SurfacePoint point = RandomSurfacePoint(world, bounds, config, rng);
				if (std::abs(point.x - bounds.center_x) < 10 && std::abs(point.z - bounds.center_z) < 10) continue;
				const int radius = 1 + (index % 2);
				const int height = ((index * 3) % 3);
				PlaceBoulder(world, bounds, point.x, point.y, point.z, radius, height, config.seed + index);
			}
		}
