		void FillDisc(ve::world::World& world, const DemoBounds& bounds, int cx, int y, int cz, int radius, BlockId block, int noise_seed)
		{
			for (int dx = -radius; dx <= radius; ++dx)
			{
				for (int dz = -radius; dz <= radius; ++dz)
				{
					const float normalized = static_cast<float>((dx * dx) + (dz * dz)) / static_cast<float>(std::max(radius * radius, 1));
					if (normalized > 1.0f) continue;
					if (normalized > 0.78f && Hash01(cx + dx, cz + dz, noise_seed) < 0.35f) continue;
					SetBlock(world, bounds, cx + dx, y, cz + dz, block);
				}
			}
		}

		void BuildMossyOutcrop(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.cave) return;
			const int cx = bounds.center_x + 52;
			const int cz = bounds.center_z - 48;
			const int base_y = FindGroundY(world, bounds, cx, cz, config.ground_y);
			const int levels = std::max(2, config.tower_height / 3);
			for (int level = 0; level < levels; ++level)
			{
				const int radius = std::max(3, 11 - (level * 3));
				const BlockId block = level % 5 == 0 ? BlockId::MossyCobblestone : (level % 3 == 0 ? BlockId::Andesite : BlockId::Stone);
				FillDisc(world, bounds, cx, base_y + 1 + level, cz, radius, block, config.seed + level);
			}
			FillDisc(world, bounds, cx, base_y + levels + 1, cz, 4, BlockId::MossBlock, config.seed + 401);
		}

		void BuildStreamCut(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.cave) return;
			for (int step = -config.terrain_radius; step <= config.terrain_radius; ++step)
			{
				const int cx = bounds.center_x + step;
				const int cz = bounds.center_z - 8 + static_cast<int>(std::round(std::sin(static_cast<float>(step) * 0.12f) * 9.0f));
				const int half_width = 1 + (std::abs(step) % 4);
				for (int offset = -half_width; offset <= half_width; ++offset)
				{
					const int x = cx;
					const int z = cz + offset;
					const int ground = FindGroundY(world, bounds, x, z, config.ground_y);
					const int carve = 1 + (half_width - std::abs(offset)) / 2;
					FillBox(world, bounds, x, ground - carve + 1, z, x, ground + 2, z, BlockId::Air);
					SetBlock(world, bounds, x, ground - carve, z, offset == 0 ? BlockId::Water : BlockId::MossyCobblestone);
					if (std::abs(offset) == half_width) SetBlock(world, bounds, x, ground - carve + 1, z, BlockId::MossBlock);
				}
			}
		}

		void CarveCavePocket(ve::world::World& world, const DemoBounds& bounds, int cx, int cy, int cz, int rx, int ry, int rz)
		{
			for (int dx = -rx; dx <= rx; ++dx)
			{
				for (int dy = -ry; dy <= ry; ++dy)
				{
					for (int dz = -rz; dz <= rz; ++dz)
					{
						const float nx = static_cast<float>(dx) / static_cast<float>(std::max(rx, 1));
						const float ny = static_cast<float>(dy) / static_cast<float>(std::max(ry, 1));
						const float nz = static_cast<float>(dz) / static_cast<float>(std::max(rz, 1));
						const float distance = (nx * nx) + (ny * ny * 1.4f) + (nz * nz);
						if (distance <= 1.0f) SetBlock(world, bounds, cx + dx, cy + dy, cz + dz, BlockId::Air);
						else if (distance <= 1.25f && Hash01(cx + dx, cz + dz, cy + dy) > 0.38f)
						{
							SetBlock(world, bounds, cx + dx, cy + dy, cz + dz, dy < 0 ? BlockId::MossyCobblestone : BlockId::Blackstone);
						}
					}
				}
			}
		}

		void PlaceCaveTorch(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z, int side)
		{
			SetBlock(world, bounds, x, y, z, BlockId::SpruceLog);
			SetBlock(world, bounds, x, y + 1, z, BlockId::GoldOre);
			SetBlock(world, bounds, x, y + 2, z, BlockId::SeaLantern);
			SetBlock(world, bounds, x + side, y + 1, z, BlockId::Terracotta);
		}

		void BuildTorchCave(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.cave) return;
			const int start_x = bounds.center_x - 76;
			const int start_z = bounds.center_z + 58;
			for (int step = 0; step < 74; ++step)
			{
				const int x = start_x + step;
				const int z = start_z + static_cast<int>(std::round(std::sin(static_cast<float>(step) * 0.18f) * 7.0f));
				const int ground = FindGroundY(world, bounds, x, z, config.ground_y);
				const int y = std::max(8, ground - 3 - (step / 18));
				CarveCavePocket(world, bounds, x, y + 2, z, 4, 4, 3);
				FillBox(world, bounds, x - 3, y - 1, z - 2, x + 3, y - 1, z + 2, step % 5 == 0 ? BlockId::MossBlock : BlockId::Stone);
				if (step % 11 == 3) PlaceCaveTorch(world, bounds, x, y, z - 3, -1);
				if (step % 17 == 8) PlaceCaveTorch(world, bounds, x, y, z + 3, 1);
			}
		}
