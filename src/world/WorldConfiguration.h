#pragma once

#include <cstddef>

namespace ve::world
{
	/**
	 * Creation-time memory settings for a world instance.
	 */
	struct WorldCreateInfo
	{
		// TODO: Add allocator/streaming policy so large worlds do not require reserving every chunk up front.
		std::size_t chunkCapacity;
	};

	/**
	 * Settings used to generate a flat square chunk grid.
	 */
	struct FlatWorldSpawnSettings
	{
		// TODO: Add seed, base height, block palette, and spawn biome to flat/procedural generation settings.
		int worldSizeChunks;
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
		std::size_t pendingWorldGenerationTaskCount;
		std::size_t reservedChunkStorageBytes;
		std::size_t levelArenaCapacityBytes;
		std::size_t levelArenaUsedBytes;
		std::size_t pendingWorldEventCount;
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
}
