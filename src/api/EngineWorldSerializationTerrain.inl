namespace
{
	[[nodiscard]] const char* ToSerializedTerrainGenerator(TerrainGenerator generator) noexcept
	{
		switch (generator)
		{
		case TerrainGenerator::Flat: return "flat";
		case TerrainGenerator::Procedural:
		default: return "procedural";
		}
	}

	[[nodiscard]] TerrainGenerator FromSerializedTerrainGenerator(const std::string& value) noexcept
	{
		if (value == "flat") return TerrainGenerator::Flat;
		return TerrainGenerator::Procedural;
	}

	[[nodiscard]] const char* ToSerializedTerrainBiome(TerrainBiome biome) noexcept
	{
		switch (biome)
		{
		case TerrainBiome::Desert: return "desert";
		case TerrainBiome::Alpine: return "alpine";
		case TerrainBiome::Temperate:
		default: return "temperate";
		}
	}

	[[nodiscard]] TerrainBiome FromSerializedTerrainBiome(const std::string& value) noexcept
	{
		if (value == "desert") return TerrainBiome::Desert;
		if (value == "alpine") return TerrainBiome::Alpine;
		return TerrainBiome::Temperate;
	}
}

void SaveWorldTerrainConfig(std::ofstream& file, const WorldConfig& world)
{
	file << "terrain_seed " << world.terrain_seed << '\n';
	file << "base_surface_height " << world.base_surface_height << '\n';
	file << "terrain_generator " << ToSerializedTerrainGenerator(world.terrain_generator) << '\n';
	file << "terrain_biome " << ToSerializedTerrainBiome(world.terrain_biome) << '\n';
	file << "terrain_palette "
		<< ToSerializedBlock(world.terrain_palette.bedrock) << ' '
		<< ToSerializedBlock(world.terrain_palette.deep_stone) << ' '
		<< ToSerializedBlock(world.terrain_palette.stone) << ' '
		<< ToSerializedBlock(world.terrain_palette.surface) << ' '
		<< ToSerializedBlock(world.terrain_palette.subsurface) << ' '
		<< ToSerializedBlock(world.terrain_palette.high_elevation_surface) << ' '
		<< ToSerializedBlock(world.terrain_palette.dry_surface_patch) << ' '
		<< ToSerializedBlock(world.terrain_palette.gravel_patch) << '\n';
}
