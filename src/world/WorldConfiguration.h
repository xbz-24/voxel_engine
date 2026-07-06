#pragma once

#include "Block.h"

#include <cstddef>
#include <cstdint>

namespace ve::world
{
	/**
	 * Creation-time memory settings for a world instance.
	 */
	enum class ChunkStoragePolicy
	{
		FixedReserve,
		GrowOnDemand
	};

	struct WorldCreateInfo
	{
		std::size_t chunkCapacity = 0;
		ChunkStoragePolicy chunkStoragePolicy = ChunkStoragePolicy::FixedReserve;
	};

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
	 * Lightweight world diagnostics useful for debug overlays and tooling.
	 */
	struct WorldMetrics
	{
		int worldSizeInChunks;
		std::size_t loadedChunkCount;
		std::size_t reservedChunkCapacity;
		std::size_t chunksNeedingMeshBuild;
		std::size_t chunksWithQueuedMeshBuild;
		std::size_t pendingChunkMeshTaskCount;
		std::size_t pendingChunkMeshUploadCount;
		std::size_t pendingWorldGenerationTaskCount;
		std::size_t reservedChunkStorageBytes;
		std::size_t levelArenaCapacityBytes;
		std::size_t levelArenaUsedBytes;
		std::size_t pendingWorldEventCount;
		ChunkStoragePolicy chunkStoragePolicy;
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
