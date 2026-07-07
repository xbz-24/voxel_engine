[[nodiscard]] bool TryLoadWorldTerrainCommand(
	WorldConfig& world,
	const std::string& command,
	std::ifstream& file)
{
	if (command == "terrain_seed")
	{
		std::uint32_t terrain_seed = world.terrain_seed;
		file >> terrain_seed;
		world.WithTerrainSeed(terrain_seed);
		return true;
	}
	if (command == "base_surface_height")
	{
		int base_surface_height = world.base_surface_height;
		file >> base_surface_height;
		world.WithBaseSurfaceHeight(base_surface_height);
		return true;
	}
	if (command == "terrain_generator")
	{
		std::string terrain_generator;
		file >> terrain_generator;
		world.WithTerrainGenerator(FromSerializedTerrainGenerator(terrain_generator));
		return true;
	}
	if (command == "terrain_biome")
	{
		std::string terrain_biome;
		file >> terrain_biome;
		world.WithTerrainBiome(FromSerializedTerrainBiome(terrain_biome));
		return true;
	}
	if (command != "terrain_palette") return false;

	int bedrock = 0;
	int deep_stone = 0;
	int stone = 0;
	int surface = 0;
	int subsurface = 0;
	int high_elevation_surface = 0;
	int dry_surface_patch = 0;
	int gravel_patch = 0;
	file >> bedrock >> deep_stone >> stone >> surface >> subsurface >>
		high_elevation_surface >> dry_surface_patch >> gravel_patch;
	world.WithTerrainPalette(TerrainPalette{
		FromSerializedBlock(bedrock),
		FromSerializedBlock(deep_stone),
		FromSerializedBlock(stone),
		FromSerializedBlock(surface),
		FromSerializedBlock(subsurface),
		FromSerializedBlock(high_elevation_surface),
		FromSerializedBlock(dry_surface_patch),
		FromSerializedBlock(gravel_patch)
	});
	return true;
}
