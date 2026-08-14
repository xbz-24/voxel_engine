#include "WorldViewRange.h"

#include "Chunk.h"
#include "WorldGridMath.h"

#include <algorithm>
#include <cstdint>

namespace ve::world
{
	namespace
	{
		[[nodiscard]] int MinimumChunkBoundary(int camera_chunk, std::int64_t radius) noexcept
		{
			return static_cast<int>(std::max(
				std::int64_t{ 0 },
				static_cast<std::int64_t>(camera_chunk) - radius));
		}

		[[nodiscard]] int MaximumChunkBoundary(int camera_chunk, std::int64_t radius, int last_chunk) noexcept
		{
			return static_cast<int>(std::min(
				static_cast<std::int64_t>(last_chunk),
				static_cast<std::int64_t>(camera_chunk) + radius));
		}
	}

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
		const std::int64_t renderDistance = std::max(
			std::int64_t{ 0 },
			static_cast<std::int64_t>(render_distance_chunks));
		return {
			MinimumChunkBoundary(cameraChunkX, renderDistance),
			MaximumChunkBoundary(cameraChunkX, renderDistance, lastChunk),
			MinimumChunkBoundary(cameraChunkZ, renderDistance),
			MaximumChunkBoundary(cameraChunkZ, renderDistance, lastChunk)
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
