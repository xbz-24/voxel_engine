		void BuildJunglePlants(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			if (!config.trees || config.tree_count <= 0) return;
			std::mt19937 rng(static_cast<std::uint32_t>(config.seed + 601));
			for (int index = 0; index < config.tree_count; ++index)
			{
				const SurfacePoint point = RandomSurfacePoint(world, bounds, config, rng);
				const float tree_roll = Hash01(point.x, point.z, config.seed + index);
				if (tree_roll > 0.86f)
				{
					PlaceEmergentTree(world, bounds, point.x, point.y, point.z, config.seed + index);
				}
				else if (tree_roll > 0.54f)
				{
					PlaceCanopyTree(world, bounds, point.x, point.y, point.z, config.seed + index);
				}
				else
				{
					PlaceUnderstory(world, bounds, point.x, point.y, point.z, config.seed + index);
				}
			}
		}

		void BuildWildlife(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			std::mt19937 rng(static_cast<std::uint32_t>(config.seed + 901));
			for (int index = 0; index < 34; ++index)
			{
				const SurfacePoint point = RandomSurfacePoint(world, bounds, config, rng);
				ClearAnimalSpace(world, bounds, point.x, point.y, point.z);
				if (index % 5 == 0) PlaceFrog(world, bounds, point.x, point.y, point.z);
				else if (index % 5 == 1) PlaceBird(world, bounds, point.x, point.y, point.z);
				else if (index % 5 == 2) PlaceTapir(world, bounds, point.x, point.y, point.z);
				else if (index % 5 == 3) PlaceCapybara(world, bounds, point.x, point.y, point.z);
				else PlaceDeer(world, bounds, point.x, point.y, point.z);
			}
		}
