#include "WorldSpawnPreparation.h"

#include "RenderBackend.h"
#include "WorldConfiguration.h"

namespace ve::world::detail
{
	WorldSpawnPreparation PrepareWorldSpawn(
		int world_size_chunks,
		ChunkGenerationMode generation_mode,
		const ve::rendering::RenderBackend* render_backend)
	{
		WorldSpawnPreparation preparation;
		const std::size_t chunk_count = ChunkCapacityForSquareWorld(world_size_chunks);
		preparation.render_meshes.reserve(chunk_count);
		if (generation_mode == ChunkGenerationMode::GenerateNow)
		{
			preparation.generated_events.reserve(chunk_count);
		}

		for (int chunk_x = 0; chunk_x < world_size_chunks; ++chunk_x)
		{
			for (int chunk_z = 0; chunk_z < world_size_chunks; ++chunk_z)
			{
				preparation.render_meshes.push_back(
					render_backend != nullptr ? render_backend->CreateMeshResource() : nullptr);
				if (generation_mode == ChunkGenerationMode::GenerateNow)
				{
					preparation.generated_events.emplace_back(ChunkGeneratedEvent{ chunk_x, chunk_z });
				}
			}
		}
		return preparation;
	}
}
