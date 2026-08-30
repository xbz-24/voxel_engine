#include "voxel/Engine.h"

#include "WorldSerializationHelpers.h"

#include <fstream>

namespace voxel::detail
{
	const char* ToSerializedTerrainGenerator(TerrainGenerator generator) noexcept
	{
		switch (generator)
		{
		case TerrainGenerator::Flat: return "flat";
		case TerrainGenerator::Procedural:
		default: return "procedural";
		}
	}

	bool TryParseSerializedTerrainGenerator(
		std::string_view value,
		TerrainGenerator& generator) noexcept
	{
		if (value == "procedural") generator = TerrainGenerator::Procedural;
		else if (value == "flat") generator = TerrainGenerator::Flat;
		else return false;
		return true;
	}

	const char* ToSerializedTerrainBiome(TerrainBiome biome) noexcept
	{
		switch (biome)
		{
		case TerrainBiome::Desert: return "desert";
		case TerrainBiome::Alpine: return "alpine";
		case TerrainBiome::Temperate:
		default: return "temperate";
		}
	}

	bool TryParseSerializedTerrainBiome(
		std::string_view value,
		TerrainBiome& biome) noexcept
	{
		if (value == "temperate") biome = TerrainBiome::Temperate;
		else if (value == "desert") biome = TerrainBiome::Desert;
		else if (value == "alpine") biome = TerrainBiome::Alpine;
		else return false;
		return true;
	}
}

namespace voxel::detail
{
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
}
