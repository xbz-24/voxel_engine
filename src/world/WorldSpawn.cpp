#include "World.h"
#include "WorldSpawnPreparation.h"

#include <cstddef>
#include <utility>

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
		SpawnPreparedGrid(settings, ChunkGenerationMode::GenerateNow);
	}

	/// Spawns a square grid of air chunks ready for async generation.
	void World::SpawnEmptyGrid(const FlatWorldSpawnSettings& settings)
	{
		SpawnPreparedGrid(settings, ChunkGenerationMode::Empty);
	}

	void World::SpawnPreparedGrid(
		const FlatWorldSpawnSettings& settings,
		ChunkGenerationMode generation_mode)
	{
		detail::WorldSpawnPreparation preparation = detail::PrepareWorldSpawn(
			settings.worldSizeChunks,
			generation_mode,
			active_render_backend_);
		_chunks.reserve(preparation.render_meshes.size());
		ResetChunkStorageForRespawn(settings.worldSizeChunks);

		std::size_t mesh_index = 0;
		for (int chunk_x = 0; chunk_x < settings.worldSizeChunks; ++chunk_x)
		{
			for (int chunk_z = 0; chunk_z < settings.worldSizeChunks; ++chunk_z)
			{
				_chunks.emplace_back(
					chunk_x,
					chunk_z,
					generation_mode,
					settings.terrainGeneration,
					std::move(preparation.render_meshes[mesh_index++]));
			}
		}
		_pendingEvents.swap(preparation.generated_events);
		++_revision;
	}

	void World::ResetChunkStorageForRespawn(int worldSizeChunks) noexcept
	{
		_chunks.clear();
		dirty_chunks_.clear();
		_pendingEvents.clear();
		_worldSize = worldSizeChunks;
		++_chunkStorageRevision;
	}
}
