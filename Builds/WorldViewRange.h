#pragma once

#include <glm/glm.hpp>

namespace ve::world
{
	/**
	 * Inclusive chunk-grid range considered for rendering.
	 */
	struct ChunkViewRange
	{
		int minChunkX;
		int maxChunkX;
		int minChunkZ;
		int maxChunkZ;
	};

	/**
	 * Builds the chunk range around the camera for a square world.
	 *
	 * @param cameraPosition Camera world position.
	 * @param worldSize Number of chunks along one world side.
	 * @param renderDistanceChunks Radius around the camera chunk.
	 * @return Inclusive chunk range clamped to the world bounds.
	 */
	ChunkViewRange BuildChunkViewRange(const glm::vec3& cameraPosition, int worldSize, int renderDistanceChunks);

	/**
	 * Checks whether a range contains at least one chunk.
	 *
	 * @param range Chunk range to test.
	 * @return True when nested loops over the range should run.
	 */
	bool HasChunks(const ChunkViewRange& range) noexcept;
}
