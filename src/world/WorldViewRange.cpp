#include "WorldViewRange.h"

#include "Chunk.h"
#include "WorldCoordinates.h"

#include <algorithm>
#include <cmath>

namespace ve::world
{
	/**
	 * Builds the chunk range around the camera for a square world.
	 *
	 * @param cameraPosition Camera world position.
	 * @param worldSize Number of chunks along one world side.
	 * @param renderDistanceChunks Radius around the camera chunk.
	 * @return Inclusive chunk range clamped to the world bounds.
	 */
	ChunkViewRange BuildChunkViewRange(const glm::vec3& cameraPosition, int worldSize, int renderDistanceChunks)
	{
		if (worldSize <= 0)
		{
			return { 0, -1, 0, -1 };
		}

		const int cameraChunkX = coordinates::FloorDiv(static_cast<int>(std::floor(cameraPosition.x)), Chunk::CHUNK_WIDTH);
		const int cameraChunkZ = coordinates::FloorDiv(static_cast<int>(std::floor(cameraPosition.z)), Chunk::CHUNK_DEPTH);
		const int lastChunk = worldSize - 1;
		return {
			std::max(0, cameraChunkX - renderDistanceChunks),
			std::min(lastChunk, cameraChunkX + renderDistanceChunks),
			std::max(0, cameraChunkZ - renderDistanceChunks),
			std::min(lastChunk, cameraChunkZ + renderDistanceChunks)
		};
	}

	/**
	 * Checks whether a range contains at least one chunk.
	 *
	 * @param range Chunk range to test.
	 * @return True when nested loops over the range should run.
	 */
	bool HasChunks(const ChunkViewRange& range) noexcept
	{
		return range.minChunkX <= range.maxChunkX && range.minChunkZ <= range.maxChunkZ;
	}
}
