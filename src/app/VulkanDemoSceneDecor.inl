		void BuildMossyRuins(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.ruins || config.ruin_count <= 0) return;
			for (int index = 0; index < config.ruin_count; ++index)
			{
				const int x = bounds.center_x - 46 + (index % 4) * 28;
				const int z = bounds.center_z + 52 - (index / 4) * 24;
				const int y = FindGroundY(world, bounds, x, z, config.ground_y);
				const int height = 1 + (index % 2);
				FillBox(world, bounds, x - 2, y + 1, z - 2, x - 2, y + height, z - 2, BlockId::MossyCobblestone);
				FillBox(world, bounds, x + 2, y + 1, z - 2, x + 2, y + 1, z - 2, BlockId::Stone);
				FillBox(world, bounds, x - 2, y + 1, z + 2, x - 2, y + 1, z + 2, BlockId::Andesite);
				if (index % 3 == 0) FillBox(world, bounds, x - 1, y + 1, z, x + 1, y + 1, z, BlockId::MossBlock);
				SetBlock(world, bounds, x, y + 1, z, BlockId::AmethystBlock);
			}
		}

		void BuildPebbleFields(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const int count = config.market_stall_count * (config.stress_blocks ? 16 : 6);
			if (count <= 0) return;
			std::mt19937 rng(static_cast<std::uint32_t>(config.seed + 701));
			const std::array<BlockId, 6> palette{ BlockId::Gravel, BlockId::Granite, BlockId::Andesite, BlockId::Diorite, BlockId::MossBlock, BlockId::OakLeaves };
			for (int index = 0; index < count; ++index)
			{
				const SurfacePoint point = RandomSurfacePoint(world, bounds, config, rng);
				if (Hash01(point.x, point.z, config.seed + index) < 0.25f) continue;
				const std::size_t palette_index = static_cast<std::size_t>(index % static_cast<int>(palette.size()));
				SetBlock(world, bounds, point.x, point.y + 1, point.z, palette[palette_index]);
			}
		}

		void BuildHeatGlints(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.lights || config.lantern_count <= 0) return;
			for (int index = 0; index < config.lantern_count; ++index)
			{
				const float angle = (static_cast<float>(index) / static_cast<float>(std::max(config.lantern_count, 1))) * 6.2831853f;
				const int radius = 18 + ((index * 11) % std::max(config.terrain_radius - 18, 1));
				const int x = bounds.center_x + static_cast<int>(std::round(std::cos(angle) * static_cast<float>(radius)));
				const int z = bounds.center_z + static_cast<int>(std::round(std::sin(angle) * static_cast<float>(radius)));
				const int y = FindGroundY(world, bounds, x, z, config.ground_y);
				SetBlock(world, bounds, x, y + 1, z, BlockId::Glass);
				if (index % 4 == 0) SetBlock(world, bounds, x, y, z, BlockId::SeaLantern);
			}
		}

		void BuildSurveyMarkers(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (config.vista_marker_count <= 0) return;
			const int radius = std::max(28, config.terrain_radius - 4);
			for (int index = 0; index < config.vista_marker_count; ++index)
			{
				const float angle = (static_cast<float>(index) / static_cast<float>(std::max(config.vista_marker_count, 1))) * 6.2831853f;
				const int x = bounds.center_x + static_cast<int>(std::round(std::cos(angle) * static_cast<float>(radius)));
				const int z = bounds.center_z + static_cast<int>(std::round(std::sin(angle) * static_cast<float>(radius)));
				const int y = FindGroundY(world, bounds, x, z, config.ground_y);
				SetBlock(world, bounds, x, y + 1, z, BlockId::SeaLantern);
				SetBlock(world, bounds, x, y + 2, z, index % 2 == 0 ? BlockId::DiamondOre : BlockId::AmethystBlock);
			}
		}

		void BuildModelStage(ve::world::World& world, const DemoBounds& bounds, int base_y, int radius)
		{
			FillBox(world, bounds,
				bounds.center_x - radius,
				base_y - 2,
				bounds.center_z - radius,
				bounds.center_x + radius,
				base_y - 2,
				bounds.center_z + radius,
				BlockId::Blackstone);
			FillBox(world, bounds,
				bounds.center_x - radius + 1,
				base_y - 1,
				bounds.center_z - radius + 1,
				bounds.center_x + radius - 1,
				base_y - 1,
				bounds.center_z + radius - 1,
				BlockId::Andesite);
			FillBox(world, bounds,
				bounds.center_x - radius + 2,
				base_y,
				bounds.center_z - 1,
				bounds.center_x + radius - 2,
				base_y,
				bounds.center_z + 1,
				BlockId::Stone);
			FillBox(world, bounds,
				bounds.center_x - 1,
				base_y,
				bounds.center_z - radius + 2,
				bounds.center_x + 1,
				base_y,
				bounds.center_z + radius - 2,
				BlockId::Stone);
		}
