		[[nodiscard]] float Hash01(int x, int z, int seed) noexcept
		{
			std::uint32_t value = static_cast<std::uint32_t>(x) * 0x9E3779B9u;
			value ^= static_cast<std::uint32_t>(z) * 0x85EBCA6Bu;
			value ^= static_cast<std::uint32_t>(seed) * 0xC2B2AE35u;
			value ^= value >> 16u;
			value *= 0x7FEB352Du;
			value ^= value >> 15u;
			return static_cast<float>(value & 0xFFFFu) / 65535.0f;
		}

		[[nodiscard]] float SignedHash(int x, int z, int seed) noexcept
		{
			return (Hash01(x, z, seed) * 2.0f) - 1.0f;
		}

		[[nodiscard]] int FindGroundY(const ve::world::World& world, const DemoBounds& bounds, int x, int z, int fallback_y)
		{
			if (x < 0 || x >= bounds.width || z < 0 || z >= bounds.depth) return fallback_y;
			for (int y = bounds.height - 2; y >= 1; --y)
			{
				const BlockId block = world.GetBlock(x, y, z);
				if (block != BlockId::Air && block != BlockId::Water && block != BlockId::Glass) return y;
			}
			return fallback_y;
		}

		[[nodiscard]] int TerrainHeightAt(const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config, int x, int z) noexcept
		{
			const float fx = static_cast<float>(x - bounds.center_x);
			const float fz = static_cast<float>(z - bounds.center_z);
			const float distance = std::sqrt((fx * fx) + (fz * fz));
			const float radius = static_cast<float>(std::max(config.terrain_radius, 1));
			const float radial_fade = std::clamp(1.0f - (distance / (radius + 42.0f)), 0.0f, 1.0f);
			const float canopy_a = std::sin((static_cast<float>(x + config.seed) * 0.037f) + (static_cast<float>(z) * 0.019f));
			const float canopy_b = std::sin((static_cast<float>(x - z) * 0.027f) + (static_cast<float>(config.seed) * 0.009f));
			const float ridge = std::cos((static_cast<float>(x) * 0.075f) + (static_cast<float>(z) * 0.041f) -
				(static_cast<float>(config.seed) * 0.007f));
			const float local = SignedHash(x / 3, z / 3, config.seed + 71) * 1.9f;
			const float height =
				static_cast<float>(config.ground_y) +
				(canopy_a * static_cast<float>(config.hill_height) * 0.34f * radial_fade) +
				(canopy_b * static_cast<float>(config.hill_height) * 0.26f) +
				(ridge * 2.8f * radial_fade) +
				local;
			return std::clamp(static_cast<int>(std::round(height)), 6, bounds.height - 10);
		}

		[[nodiscard]] BlockId SurfaceBlockFor(const VulkanMinecraftDemoSceneConfig& config, int x, int z, int surface_y) noexcept
		{
			const float vein_width = 0.014f + (static_cast<float>(config.ore_richness) * 0.0006f);
			const float root_line = std::abs(std::sin((static_cast<float>(x) * 0.24f) + (static_cast<float>(z) * 0.17f) +
				(static_cast<float>(config.seed) * 0.005f)));
			const float warm_patch = Hash01(x / 2, z / 2, config.seed + 19);
			if (root_line < vein_width && warm_patch > 0.30f) return warm_patch > 0.68f ? BlockId::MossyCobblestone : BlockId::MossBlock;
			if (surface_y > config.ground_y + (config.hill_height / 2) && warm_patch < 0.16f) return BlockId::Stone;
			if (warm_patch < 0.16f) return BlockId::MossBlock;
			if (warm_patch > 0.94f) return BlockId::OakLeaves;
			return BlockId::Grass;
		}

		void SetJungleColumn(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config, int x, int z, int surface_y)
		{
			const float layer_noise = Hash01(x / 5, z / 5, config.seed + 43);
			const BlockId base = layer_noise < 0.50f ? BlockId::Stone : (layer_noise < 0.74f ? BlockId::Andesite : BlockId::Deepslate);
			const BlockId stratum = layer_noise < 0.55f ? BlockId::Dirt : (layer_noise < 0.80f ? BlockId::MossyCobblestone : BlockId::Granite);
			const BlockId humus = layer_noise < 0.42f ? BlockId::Dirt : BlockId::MossBlock;
			FillColumnRange(world, bounds, x, 0, z, std::max(0, surface_y - 15), BlockId::Deepslate);
			FillColumnRange(world, bounds, x, std::max(0, surface_y - 13), z, surface_y - 8, base);
			FillColumnRange(world, bounds, x, surface_y - 7, z, surface_y - 4, stratum);
			FillColumnRange(world, bounds, x, surface_y - 3, z, surface_y - 1, humus);
			SetBlock(world, bounds, x, surface_y, z, SurfaceBlockFor(config, x, z, surface_y));
		}

		void BuildJungleTerrain(ve::world::World& world, const DemoBounds& bounds, const VulkanMinecraftDemoSceneConfig& config)
		{
			const int margin = 3;
			for (int x = margin; x < bounds.width - margin; ++x)
			{
				for (int z = margin; z < bounds.depth - margin; ++z)
				{
					const int surface_y = TerrainHeightAt(bounds, config, x, z);
					FillColumnRange(world, bounds, x, surface_y + 1, z, bounds.height - 1, BlockId::Air);
					SetJungleColumn(world, bounds, config, x, z, surface_y);
				}
			}
		}
