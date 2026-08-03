		};

		Kind kind = Kind::SetBlock;
		BlockPosition position{};
		BlockBox box{};
		Block block = Block::Air;
	};

	/** Terrain generation strategy used when a world is initialized. */
	enum class TerrainGenerator
	{
		Procedural,
		Flat
	};

	/** Biome hint consumed by procedural terrain generation. */
	enum class TerrainBiome
	{
		Temperate,
		Desert,
		Alpine
	};

	/** Block palette used by terrain generation layers and patches. */
	struct TerrainPalette
	{
		Block bedrock = Bedrock;
		Block deep_stone = Deepslate;
		Block stone = Stone;
		Block surface = Grass;
		Block subsurface = Dirt;
		Block high_elevation_surface = Snow;
		Block dry_surface_patch = Sand;
		Block gravel_patch = Gravel;
	};
}
