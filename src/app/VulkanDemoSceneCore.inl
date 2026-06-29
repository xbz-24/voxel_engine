namespace ve::engine
{
	namespace
	{
		using ve::blocks::BlockId;
		using ve::rendering::VulkanMinecraftDemoSceneConfig;

		struct DemoBounds
		{
			int width = 0;
			int depth = 0;
			int height = ve::world::terrain::ChunkHeight;
			int center_x = 0;
			int center_z = 0;
		};

		struct SurfacePoint
		{
			int x = 0;
			int y = 0;
			int z = 0;
		};

		[[nodiscard]] VulkanMinecraftDemoSceneConfig Sanitize(VulkanMinecraftDemoSceneConfig config) noexcept
		{
			config.ground_y = std::clamp(config.ground_y, 34, 76);
			config.terrain_radius = std::clamp(config.terrain_radius, 48, 140);
			config.hill_height = std::clamp(config.hill_height, 4, 22);
			config.house_count = std::clamp(config.house_count, 0, 120);
			config.tree_count = std::clamp(config.tree_count, 0, 520);
			config.farm_rows = std::clamp(config.farm_rows, 0, 48);
			config.water_radius = std::clamp(config.water_radius, 0, 14);
			config.tower_height = std::clamp(config.tower_height, 2, 14);
			config.lantern_count = std::clamp(config.lantern_count, 0, 40);
			config.ore_richness = std::clamp(config.ore_richness, 0, 56);
			config.market_stall_count = std::clamp(config.market_stall_count, 0, 220);
			config.floating_island_count = 0;
			config.ruin_count = std::clamp(config.ruin_count, 0, 8);
			config.bridge_count = 0;
			config.vista_marker_count = std::clamp(config.vista_marker_count, 0, 12);
			config.village = false;
			config.farms = false;
			config.market = false;
			config.floating_islands = false;
			config.beacon = false;
			return config;
		}

		[[nodiscard]] DemoBounds BoundsFor(const ve::world::World& world) noexcept
		{
			const ve::world::WorldMetrics metrics = world.Metrics();
			const int width = metrics.worldSizeChunks * Chunk::CHUNK_WIDTH;
			const int depth = metrics.worldSizeChunks * Chunk::CHUNK_DEPTH;
			return DemoBounds{ width, depth, ve::world::terrain::ChunkHeight, width / 2, depth / 2 };
		}

		[[nodiscard]] bool IsInside(const DemoBounds& bounds, int x, int y, int z) noexcept
		{
			return x >= 0 && x < bounds.width && y >= 0 && y < bounds.height && z >= 0 && z < bounds.depth;
		}

		void SetBlock(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z, BlockId block)
		{
			if (IsInside(bounds, x, y, z)) world.SetBlock(x, y, z, block);
		}

		void FillBox(ve::world::World& world, const DemoBounds& bounds, int min_x, int min_y, int min_z, int max_x, int max_y, int max_z, BlockId block)
		{
			const int begin_x = std::max(0, std::min(min_x, max_x));
			const int begin_y = std::max(0, std::min(min_y, max_y));
			const int begin_z = std::max(0, std::min(min_z, max_z));
			const int end_x = std::min(bounds.width - 1, std::max(min_x, max_x));
			const int end_y = std::min(bounds.height - 1, std::max(min_y, max_y));
			const int end_z = std::min(bounds.depth - 1, std::max(min_z, max_z));
			for (int x = begin_x; x <= end_x; ++x)
			{
				for (int y = begin_y; y <= end_y; ++y)
				{
					for (int z = begin_z; z <= end_z; ++z)
					{
						world.SetBlock(x, y, z, block);
					}
				}
			}
		}

		void FillColumnRange(ve::world::World& world, const DemoBounds& bounds, int x, int min_y, int z, int max_y, BlockId block)
		{
			if (max_y < min_y) return;
			FillBox(world, bounds, x, min_y, z, x, max_y, z, block);
		}
