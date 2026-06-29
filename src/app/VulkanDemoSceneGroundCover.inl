		void BuildGroundCover(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (config.farm_rows <= 0) return;
			const float threshold = 0.006f + (static_cast<float>(config.farm_rows) * 0.0018f);
			const int radius_sq = config.terrain_radius * config.terrain_radius;
			for (int x = bounds.center_x - config.terrain_radius; x <= bounds.center_x + config.terrain_radius; ++x)
			{
				for (int z = bounds.center_z - config.terrain_radius; z <= bounds.center_z + config.terrain_radius; ++z)
				{
					const int dx = x - bounds.center_x;
					const int dz = z - bounds.center_z;
					if ((dx * dx) + (dz * dz) > radius_sq) continue;
					const float wave = std::sin((static_cast<float>(x) * 0.72f) + (static_cast<float>(z) * 0.22f) + (static_cast<float>(config.seed) * 0.01f));
					if (std::abs(wave) > threshold) continue;
					if (Hash01(x, z, config.seed + 101) < 0.48f) continue;
					const int y = FindGroundY(world, bounds, x, z, config.ground_y);
					const float flower = Hash01(x, z, config.seed + 102);
					const BlockId block = flower < 0.035f ? BlockId::DiamondOre :
						(flower < 0.075f ? BlockId::AmethystBlock : (flower < 0.16f ? BlockId::HayBlock : (flower < 0.55f ? BlockId::MossBlock : BlockId::OakLeaves)));
					SetBlock(world, bounds, x, y + 1, z, block);
				}
			}
		}

		void BuildLagoon(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.water || config.water_radius <= 0) return;
			const int cx = bounds.center_x - 26;
			const int cz = bounds.center_z + 20;
			const int radius = config.water_radius;
			const int water_y = config.ground_y - 3;
			for (int x = cx - radius - 7; x <= cx + radius + 7; ++x)
			{
				for (int z = cz - radius - 7; z <= cz + radius + 7; ++z)
				{
					const int dx = x - cx;
					const int dz = z - cz;
					const int distance_sq = (dx * dx) + (dz * dz);
					if (distance_sq > (radius + 7) * (radius + 7)) continue;
					const bool wet = distance_sq <= radius * radius;
					const bool shore = !wet && distance_sq <= (radius + 3) * (radius + 3);
					const int surface_y = wet ? water_y - 1 : (shore ? water_y : FindGroundY(world, bounds, x, z, config.ground_y));
					FillBox(world, bounds, x, surface_y - 7, z, x, bounds.height - 1, z, BlockId::Air);
					SetJungleColumn(world, bounds, config, x, z, surface_y);
					if (wet)
					{
						SetBlock(world, bounds, x, water_y, z, BlockId::Water);
					}
					else if (shore)
					{
						SetBlock(world, bounds, x, surface_y, z, Hash01(x, z, config.seed + 121) < 0.35f ? BlockId::MossBlock : BlockId::Grass);
					}
				}
			}
		}
