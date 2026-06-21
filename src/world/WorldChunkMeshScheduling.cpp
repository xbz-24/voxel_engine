#include "World.h"

#include "WorldViewRange.h"

#include <utility>

namespace ve::world
{
	/// Uploads mesh outputs completed by background workers.
	void World::UploadReadyChunkMeshes(ve::world::mesh::ChunkMeshPipeline& meshPipeline)
	{
		meshPipeline.CollectCompletedBuilds();
		for (ve::world::mesh::ChunkMeshBuildOutput& output : meshPipeline.DrainUploadBacklog())
		{
			TryUploadChunkMeshOutput(std::move(output));
		}
	}

	/// Queues visible generated chunks that need fresh CPU mesh data.
	void World::ScheduleVisibleChunkMeshes(const ve::blocks::BlockRegistry& blockRegistry, ve::world::mesh::ChunkMeshPipeline& meshPipeline, const glm::vec3& cameraPosition, int renderDistanceChunks)
	{
		const ChunkViewRange range = BuildChunkViewRange(cameraPosition, _worldSize, renderDistanceChunks);
		if (!HasChunks(range)) return;
		for (int chunkX = range.minChunkX; chunkX <= range.maxChunkX; chunkX++)
		{
			for (int chunkZ = range.minChunkZ; chunkZ <= range.maxChunkZ; chunkZ++)
			{
				Chunk* chunk = FindChunk(chunkX, chunkZ);
				if (!chunk || !chunk->IsGenerated() || !chunk->TryReserveMeshBuild()) continue;
				auto request = CaptureChunkMeshBuildRequest(chunkX, chunkZ);
				if (request) meshPipeline.RequestBuild(std::move(*request), blockRegistry);
			}
		}
	}
}
