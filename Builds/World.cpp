#include "World.h"

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
		  _chunks(ChunkAllocator(&_levelSpawn.MemoryResource()))
	{
		_chunks.reserve(chunkCount);
	}

	void World::SpawnFlatGrid(int worldSize)
	{
		for (int x = 0; x < worldSize; x++)
		{
			for (int z = 0; z < worldSize; z++)
			{
				_chunks.emplace_back(x, z);
			}
		}
	}

	void World::Draw(Cube& cubeManager)
	{
		for (Chunk& chunk : _chunks)
		{
			chunk.Draw(cubeManager);
		}
	}
}

