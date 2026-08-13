#pragma once

#include "Block.h"
#include "WorldCreationTypes.h"

#include <cstdint>

namespace ve::world
{
	enum class TerrainSpawnBiome
	{
		Temperate,
		Desert,
		Alpine
	};

	enum class TerrainGeneratorKind
	{
		Procedural,
		Flat
	};

	struct TerrainBlockPalette
	{
		ve::blocks::BlockId bedrockBlock = ve::blocks::BlockId::Bedrock;
		ve::blocks::BlockId deepStoneBlock = ve::blocks::BlockId::Deepslate;
		ve::blocks::BlockId stoneBlock = ve::blocks::BlockId::Stone;
		ve::blocks::BlockId surfaceBlock = ve::blocks::BlockId::Grass;
		ve::blocks::BlockId subsurfaceBlock = ve::blocks::BlockId::Dirt;
		ve::blocks::BlockId highElevationSurfaceBlock = ve::blocks::BlockId::Snow;
		ve::blocks::BlockId drySurfacePatchBlock = ve::blocks::BlockId::Sand;
		ve::blocks::BlockId gravelPatchBlock = ve::blocks::BlockId::Gravel;
	};

	struct TerrainGenerationSettings
	{
		std::uint32_t terrainSeed = 0;
		int baseSurfaceHeight = 40;
		TerrainSpawnBiome spawnBiome = TerrainSpawnBiome::Temperate;
		TerrainGeneratorKind terrainGenerator = TerrainGeneratorKind::Procedural;
		TerrainBlockPalette blockPalette{};
	};

	/**
	 * Settings used to generate a flat square chunk grid.
	 */
	struct FlatWorldSpawnSettings
	{
		int worldSizeChunks = 0;
		TerrainGenerationSettings terrainGeneration{};
	};

	/**
	 * Calculates chunk storage needed by a square world.
	 *
	 * @param worldSizeChunks Number of chunks on one side of the square world.
	 * @return Total chunks needed to hold the square world.
	 */
	std::size_t ChunkCapacityForSquareWorld(int worldSizeChunks) noexcept;

	/**
	 * Builds creation settings sized for a square world.
	 *
	 * @param worldSizeChunks Number of chunks on one side of the square world.
	 * @return Creation info with enough chunk capacity for that world.
	 */
	WorldCreateInfo CreateInfoForSquareWorld(int worldSizeChunks) noexcept;

	/**
	 * Builds creation settings for chunk storage that grows as chunks stream in.
	 *
	 * @return Creation info that avoids reserving an entire world grid up front.
	 */
	WorldCreateInfo CreateInfoForStreamingWorld() noexcept;
}
