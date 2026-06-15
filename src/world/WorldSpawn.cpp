#include "World.h"

namespace ve::world
{
	/// Spawns a square grid of generated chunks.
	void World::SpawnFlatGrid(int worldSize)
	{
		SpawnFlatGrid(FlatWorldSpawnSettings{ worldSize });
	}

	/// Spawns a square grid of generated chunks.
	void World::SpawnFlatGrid(const FlatWorldSpawnSettings& settings)
	{
		_chunks.clear();
		_worldSize = settings.worldSizeChunks;
		for (int x = 0; x < settings.worldSizeChunks; x++)
		{
			for (int z = 0; z < settings.worldSizeChunks; z++)
			{
				_chunks.emplace_back(x, z, ChunkGenerationMode::GenerateNow);
				RecordChunkGenerated(x, z);
			}
		}
	}

	/// Spawns a square grid of air chunks ready for async generation.
	void World::SpawnEmptyGrid(const FlatWorldSpawnSettings& settings)
	{
		_chunks.clear();
		_worldSize = settings.worldSizeChunks;
		for (int x = 0; x < settings.worldSizeChunks; x++)
			for (int z = 0; z < settings.worldSizeChunks; z++)
				_chunks.emplace_back(x, z, ChunkGenerationMode::Empty);
	}
}
