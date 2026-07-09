#include "WorldViewRange.h"

#include "Chunk.h"
#include "WorldGridMath.h"

#include <algorithm>

namespace ve::world
{
	/**
	 * Builds the chunk range around the camera for a square world.
	 *
	 * @param cameraPosition Camera world position.
	 * @param worldSize Number of chunks along one world side.
	 * @param render_distance_chunks Radius around the camera chunk.
	 * @return Inclusive chunk range clamped to the world bounds.
	 */
	ChunkViewRange BuildChunkViewRange(const glm::vec3& cameraPosition, int worldSize, int render_distance_chunks)
	{
		if (worldSize <= 0)
		{
			return { 0, -1, 0, -1 };
		}

		const int cameraChunkX = grid::ChunkXFromWorld(cameraPosition.x);
		const int cameraChunkZ = grid::ChunkZFromWorld(cameraPosition.z);
		const int lastChunk = worldSize - 1;
		return {
			std::max(0, cameraChunkX - render_distance_chunks),
			std::min(lastChunk, cameraChunkX + render_distance_chunks),
			std::max(0, cameraChunkZ - render_distance_chunks),
			std::min(lastChunk, cameraChunkZ + render_distance_chunks)
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
