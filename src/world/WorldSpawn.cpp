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
		for (int chunk_x = 0; chunk_x < settings.worldSizeChunks; chunk_x++)
		{
			for (int chunk_z = 0; chunk_z < settings.worldSizeChunks; chunk_z++)
			{
				_chunks.emplace_back(chunk_x, chunk_z, ChunkGenerationMode::GenerateNow);
				RecordChunkGenerated(chunk_x, chunk_z);
			}
		}
		++_revision;
	}

	/// Spawns a square grid of air chunks ready for async generation.
	void World::SpawnEmptyGrid(const FlatWorldSpawnSettings& settings)
	{
		_chunks.clear();
		_worldSize = settings.worldSizeChunks;
		for (int chunk_x = 0; chunk_x < settings.worldSizeChunks; chunk_x++)
			for (int chunk_z = 0; chunk_z < settings.worldSizeChunks; chunk_z++)
				_chunks.emplace_back(chunk_x, chunk_z, ChunkGenerationMode::Empty);
		++_revision;
	}
}
