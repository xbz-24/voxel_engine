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

	/**
	 * Creates an empty world from explicit memory settings.
	 *
	 * @param createInfo Chunk capacity and arena sizing data.
	 */
	World::World(const WorldCreateInfo& createInfo)
		: World(createInfo.chunkCapacity)
	{
	}

	/**
	 * Creates an empty world with enough arena memory for the requested chunks.
	 *
	 * @param chunkCount Number of chunks to reserve in the level arena.
	 */
	World::World(std::size_t chunkCount)
		: _levelSpawn(EstimateWorldArenaBytes(chunkCount)),
		  _chunks(ChunkAllocator(&_levelSpawn.MemoryResource())),
		  _worldSize(0)
	{
		_chunks.reserve(chunkCount);
	}

	/**
	 * Spawns a square grid of generated chunks.
	 *
	 * @param worldSize Number of chunks along each world side.
	 */
	void World::SpawnFlatGrid(int worldSize)
	{
		SpawnFlatGrid(FlatWorldSpawnSettings{ worldSize });
	}

	/**
	 * Spawns a square grid of generated chunks.
	 *
	 * @param settings Spawn settings containing the world size in chunks.
	 */
	void World::SpawnFlatGrid(const FlatWorldSpawnSettings& settings)
	{
		_chunks.clear();
		_worldSize = settings.worldSizeChunks;
		for (int x = 0; x < settings.worldSizeChunks; x++)
		{
			for (int z = 0; z < settings.worldSizeChunks; z++)
			{
				_chunks.emplace_back(x, z);
			}
		}
	}

	/**
	 * Returns loaded chunk and capacity diagnostics.
	 *
	 * @return Snapshot of world size and chunk storage counts.
	 */
	WorldMetrics World::Metrics() const noexcept
	{
		return WorldMetrics{ _worldSize, _chunks.size(), _chunks.capacity() };
	}
}
