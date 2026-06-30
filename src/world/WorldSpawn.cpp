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
		ResetChunkStorageForRespawn(settings.worldSizeChunks);
		for (int chunkCoordinateX = 0; chunkCoordinateX < settings.worldSizeChunks; chunkCoordinateX++)
		{
			for (int chunkCoordinateZ = 0; chunkCoordinateZ < settings.worldSizeChunks; chunkCoordinateZ++)
			{
				_chunks.emplace_back(
					chunkCoordinateX,
					chunkCoordinateZ,
					ChunkGenerationMode::GenerateNow,
					settings.terrainGeneration,
					CreateChunkRenderMeshResource());
				RecordChunkGenerated(chunkCoordinateX, chunkCoordinateZ);
			}
		}
		++_revision;
	}

	/// Spawns a square grid of air chunks ready for async generation.
	void World::SpawnEmptyGrid(const FlatWorldSpawnSettings& settings)
	{
		ResetChunkStorageForRespawn(settings.worldSizeChunks);
		for (int chunkCoordinateX = 0; chunkCoordinateX < settings.worldSizeChunks; chunkCoordinateX++)
			for (int chunkCoordinateZ = 0; chunkCoordinateZ < settings.worldSizeChunks; chunkCoordinateZ++)
				_chunks.emplace_back(
					chunkCoordinateX,
					chunkCoordinateZ,
					ChunkGenerationMode::Empty,
					ve::world::TerrainGenerationSettings{},
					CreateChunkRenderMeshResource());
		++_revision;
	}

	void World::ResetChunkStorageForRespawn(int worldSizeChunks)
	{
		_chunks.clear();
		_pendingEvents.clear();
		_worldSize = worldSizeChunks;
	}
}
