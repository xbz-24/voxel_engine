#include "WorldSerializationHelpers.h"

#include <array>
#include <cstdint>

namespace voxel::detail
{
	WorldCommandLoadResult TryLoadWorldTerrainCommand(
		WorldConfig& world,
		const SerializedWorldLine& tokens) noexcept
	{
		if (tokens[0] == "terrain_seed")
		{
			std::uint32_t terrain_seed = 0;
			if (tokens.size() != 2 || !TryParseSerializedUInt32(tokens[1], terrain_seed))
				return WorldCommandLoadResult::Invalid;
			world.WithTerrainSeed(terrain_seed);
			return WorldCommandLoadResult::Loaded;
		}
		if (tokens[0] == "base_surface_height")
		{
			int base_surface_height = 0;
			if (tokens.size() != 2 || !TryParseSerializedInt(tokens[1], base_surface_height))
				return WorldCommandLoadResult::Invalid;
			world.WithBaseSurfaceHeight(base_surface_height);
			return WorldCommandLoadResult::Loaded;
		}
		if (tokens[0] == "terrain_generator")
		{
			TerrainGenerator generator = TerrainGenerator::Procedural;
			if (tokens.size() != 2 || !TryParseSerializedTerrainGenerator(tokens[1], generator))
				return WorldCommandLoadResult::Invalid;
			world.WithTerrainGenerator(generator);
			return WorldCommandLoadResult::Loaded;
		}
		if (tokens[0] == "terrain_biome")
		{
			TerrainBiome biome = TerrainBiome::Temperate;
			if (tokens.size() != 2 || !TryParseSerializedTerrainBiome(tokens[1], biome))
				return WorldCommandLoadResult::Invalid;
			world.WithTerrainBiome(biome);
			return WorldCommandLoadResult::Loaded;
		}
		if (tokens[0] != "terrain_palette") return WorldCommandLoadResult::Unknown;
		if (tokens.size() != 9) return WorldCommandLoadResult::Invalid;

		std::array<Block, 8> blocks{};
		for (std::size_t index = 0; index < blocks.size(); ++index)
		{
			if (!TryParseSerializedBlock(tokens[index + 1], blocks[index]))
				return WorldCommandLoadResult::Invalid;
		}
		world.WithTerrainPalette(TerrainPalette{
			blocks[0], blocks[1], blocks[2], blocks[3],
			blocks[4], blocks[5], blocks[6], blocks[7]
		});
		return WorldCommandLoadResult::Loaded;
	}
}
