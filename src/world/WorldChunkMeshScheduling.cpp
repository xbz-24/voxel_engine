#include "World.h"

#include "ChunkTerrain.h"
#include "WorldCoordinates.h"
#include "WorldViewRange.h"

#include <algorithm>
#include <cmath>
#include <vector>
#include <utility>

namespace ve::world
{
	namespace
	{
		constexpr int kMaxChunkMeshRequestsPerFrame = 32;

		struct PrioritizedChunk
		{
			int chunk_x = 0;
			int chunk_z = 0;
			int distance_squared = 0;
		};

		int CameraChunkCoordinate(float coordinate) noexcept
		{
			return coordinates::FloorDiv(static_cast<int>(std::floor(coordinate)), terrain::ChunkWidth);
		}
	}

	/// Uploads mesh outputs completed by background workers.
	void World::UploadReadyChunkMeshes(ve::world::mesh::ChunkMeshPipeline& meshPipeline)
	{
		meshPipeline.CollectCompletedBuilds();
		auto upload_backlog = meshPipeline.DrainUploadBacklog();
		for (ve::world::mesh::ChunkMeshBuildOutput& output : upload_backlog)
		{
			TryUploadChunkMeshOutput(std::move(output));
		}
	}

	/// Queues visible generated chunks that need fresh CPU mesh data.
	void World::ScheduleVisibleChunkMeshes(const ve::blocks::BlockRegistry& blockRegistry, ve::world::mesh::ChunkMeshPipeline& meshPipeline, const glm::vec3& cameraPosition, int renderDistanceChunks)
	{
		const ChunkViewRange range = BuildChunkViewRange(cameraPosition, _worldSize, renderDistanceChunks);
		if (!HasChunks(range)) return;
		const int camera_chunk_x = CameraChunkCoordinate(cameraPosition.x);
		const int camera_chunk_z = CameraChunkCoordinate(cameraPosition.z);
		std::vector<PrioritizedChunk> candidates;
		candidates.reserve(static_cast<std::size_t>((range.maxChunkX - range.minChunkX + 1) * (range.maxChunkZ - range.minChunkZ + 1)));
		for (int chunkX = range.minChunkX; chunkX <= range.maxChunkX; chunkX++)
		{
			for (int chunkZ = range.minChunkZ; chunkZ <= range.maxChunkZ; chunkZ++)
			{
				const Chunk* chunk = FindChunk(chunkX, chunkZ);
				if (!chunk || !chunk->IsGenerated() || !chunk->NeedsMeshBuild()) continue;
				const int dx = chunkX - camera_chunk_x;
				const int dz = chunkZ - camera_chunk_z;
				candidates.push_back(PrioritizedChunk{ chunkX, chunkZ, dx * dx + dz * dz });
			}
		}
		std::sort(candidates.begin(), candidates.end(), [](const PrioritizedChunk& left, const PrioritizedChunk& right)
		{
			if (left.distance_squared != right.distance_squared) return left.distance_squared < right.distance_squared;
			if (left.chunk_x != right.chunk_x) return left.chunk_x < right.chunk_x;
			return left.chunk_z < right.chunk_z;
		});

		int scheduled_count = 0;
		for (const PrioritizedChunk& candidate : candidates)
		{
			if (scheduled_count >= kMaxChunkMeshRequestsPerFrame) break;
			Chunk* chunk = FindChunk(candidate.chunk_x, candidate.chunk_z);
			if (!chunk || !chunk->TryReserveMeshBuild()) continue;
			auto request = CaptureChunkMeshBuildRequest(candidate.chunk_x, candidate.chunk_z);
			if (request && meshPipeline.RequestBuild(std::move(*request), blockRegistry))
			{
				++scheduled_count;
			}
			else
			{
				chunk->CancelMeshBuildReservation();
			}
		}
	}
}
