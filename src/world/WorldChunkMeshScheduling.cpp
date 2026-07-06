#include "World.h"

#include "ChunkTerrain.h"
#include "WorldCoordinates.h"
#include "WorldViewRange.h"

#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>
#include <utility>

namespace ve::world
{
	namespace
	{
		constexpr int kMaxChunkMeshRequestsPerFrame = 32;

		struct PrioritizedChunk
		{
			int chunkCoordinateX = 0;
			int chunkCoordinateZ = 0;
			int distanceFromCameraSquared = 0;
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
		std::vector<ve::world::mesh::ChunkMeshBuildOutput> uploadBacklog = meshPipeline.DrainUploadBacklog();
		for (ve::world::mesh::ChunkMeshBuildOutput& output : uploadBacklog)
		{
			TryUploadChunkMeshOutput(std::move(output));
		}
	}

	/// Queues visible generated chunks that need fresh CPU mesh data.
	void World::ScheduleVisibleChunkMeshes(const ve::blocks::BlockRegistry& blockRegistry, ve::world::mesh::ChunkMeshPipeline& meshPipeline, const glm::vec3& cameraPosition, int render_distance_chunks)
	{
		const ChunkViewRange range = BuildChunkViewRange(cameraPosition, _worldSize, render_distance_chunks);
		if (!HasChunks(range)) return;
		const int cameraChunkCoordinateX = CameraChunkCoordinate(cameraPosition.x);
		const int cameraChunkCoordinateZ = CameraChunkCoordinate(cameraPosition.z);
		std::vector<PrioritizedChunk> candidates;
		candidates.reserve(static_cast<std::size_t>((range.maxChunkX - range.minChunkX + 1) * (range.maxChunkZ - range.minChunkZ + 1)));
		for (int chunkX = range.minChunkX; chunkX <= range.maxChunkX; chunkX++)
		{
			for (int chunkZ = range.minChunkZ; chunkZ <= range.maxChunkZ; chunkZ++)
			{
				const Chunk* chunk = FindChunk(chunkX, chunkZ);
				if (!chunk || !chunk->IsGenerated() || !chunk->NeedsMeshBuild()) continue;
				const int chunkDistanceFromCameraX = chunkX - cameraChunkCoordinateX;
				const int chunkDistanceFromCameraZ = chunkZ - cameraChunkCoordinateZ;
				candidates.push_back(PrioritizedChunk{
					chunkX,
					chunkZ,
					(chunkDistanceFromCameraX * chunkDistanceFromCameraX) +
						(chunkDistanceFromCameraZ * chunkDistanceFromCameraZ)
				});
			}
		}
		std::sort(candidates.begin(), candidates.end(), [](const PrioritizedChunk& left, const PrioritizedChunk& right)
		{
			if (left.distanceFromCameraSquared != right.distanceFromCameraSquared)
			{
				return left.distanceFromCameraSquared < right.distanceFromCameraSquared;
			}
			if (left.chunkCoordinateX != right.chunkCoordinateX) return left.chunkCoordinateX < right.chunkCoordinateX;
			return left.chunkCoordinateZ < right.chunkCoordinateZ;
		});

		int scheduledBuildCount = 0;
		for (const PrioritizedChunk& candidate : candidates)
		{
			if (scheduledBuildCount >= kMaxChunkMeshRequestsPerFrame) break;
			Chunk* chunk = FindChunk(candidate.chunkCoordinateX, candidate.chunkCoordinateZ);
			if (!chunk || !chunk->TryReserveMeshBuild()) continue;
			std::optional<ve::world::mesh::ChunkMeshBuildRequest> request =
				CaptureChunkMeshBuildRequest(candidate.chunkCoordinateX, candidate.chunkCoordinateZ);
			if (request && meshPipeline.RequestBuild(std::move(*request), blockRegistry))
			{
				++scheduledBuildCount;
			}
			else
			{
				chunk->CancelMeshBuildReservation();
			}
		}
	}
}
