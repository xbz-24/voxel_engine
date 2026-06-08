#include "World.h"

#include "WorldCoordinates.h"
#include "WorldVisibility.h"

#include <algorithm>
#include <cmath>

namespace ve::world
{
	namespace
	{
		/**
		 * Estimates arena size for chunk storage.
		 *
		 * @param chunkCount Number of chunks expected to be spawned.
		 * @return Byte count reserved for level chunk containers.
		 */
		std::size_t EstimateWorldArenaBytes(std::size_t chunkCount)
		{
			return chunkCount * sizeof(Chunk);
		}
	}

	World::World(std::size_t chunkCount)
		: _levelSpawn(EstimateWorldArenaBytes(chunkCount)),
		  _chunks(ChunkAllocator(&_levelSpawn.MemoryResource())),
		  _worldSize(0)
	{
		_chunks.reserve(chunkCount);
	}

	void World::SpawnFlatGrid(int worldSize)
	{
		_worldSize = worldSize;
		for (int x = 0; x < worldSize; x++)
		{
			for (int z = 0; z < worldSize; z++)
			{
				_chunks.emplace_back(x, z);
			}
		}
	}

	void World::Draw(const ve::blocks::BlockRegistry& blockRegistry, const glm::vec3& cameraPosition, const glm::vec3& cameraForward, int renderDistanceChunks)
	{
		const int cameraChunkX = coordinates::FloorDiv(static_cast<int>(std::floor(cameraPosition.x)), Chunk::CHUNK_WIDTH);
		const int cameraChunkZ = coordinates::FloorDiv(static_cast<int>(std::floor(cameraPosition.z)), Chunk::CHUNK_DEPTH);
		const int minChunkX = std::max(0, cameraChunkX - renderDistanceChunks);
		const int maxChunkX = std::min(_worldSize - 1, cameraChunkX + renderDistanceChunks);
		const int minChunkZ = std::max(0, cameraChunkZ - renderDistanceChunks);
		const int maxChunkZ = std::min(_worldSize - 1, cameraChunkZ + renderDistanceChunks);

		for (int chunkX = minChunkX; chunkX <= maxChunkX; chunkX++)
		{
			for (int chunkZ = minChunkZ; chunkZ <= maxChunkZ; chunkZ++)
			{
				if (!visibility::IsChunkInView(cameraPosition, cameraForward, chunkX, chunkZ))
				{
					continue;
				}

				Chunk* chunk = FindChunk(chunkX, chunkZ);
				if (!chunk)
				{
					continue;
				}

				chunk->Draw(
					blockRegistry,
					FindChunk(chunkX - 1, chunkZ),
					FindChunk(chunkX + 1, chunkZ),
					FindChunk(chunkX, chunkZ - 1),
					FindChunk(chunkX, chunkZ + 1));
			}
		}
	}

}
