WorldConfig& WorldConfig::WithTerrainSeed(std::uint32_t value) noexcept
{
	terrain_seed = value;
	return *this;
}

WorldConfig& WorldConfig::WithBaseSurfaceHeight(int value) noexcept
{
	base_surface_height = value;
	return *this;
}

WorldConfig& WorldConfig::WithTerrainGenerator(TerrainGenerator value) noexcept
{
	terrain_generator = value;
	return *this;
}

WorldConfig& WorldConfig::WithTerrainBiome(TerrainBiome value) noexcept
{
	terrain_biome = value;
	return *this;
}

WorldConfig& WorldConfig::WithTerrainPalette(TerrainPalette value) noexcept
{
	terrain_palette = value;
	return *this;
}
