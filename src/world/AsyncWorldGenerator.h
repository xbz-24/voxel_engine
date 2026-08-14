#pragma once

#include "BackgroundTaskQueue.h"
#include "ChunkTerrain.h"
#include "CoreTypes.h"
#include "ThreadSafeMessageQueue.h"
#include "WorldConfiguration.h"

#include <atomic>
#include <cstdint>

namespace ve::world::generation
{
	struct ChunkGenerationRequest
	{
		int chunkCoordinateX = 0;
		int chunkCoordinateZ = 0;
		TerrainGenerationSettings terrainGeneration{};
		std::uint64_t chunkStorageRevision = 0;
	};

	struct ChunkGenerationResult
	{
		int chunkCoordinateX = 0;
		int chunkCoordinateZ = 0;
		ve::core::StaticArray<ve::blocks::BlockId, terrain::ChunkBlockCount> blocks{};
		std::uint64_t chunkStorageRevision = 0;
	};

	class AsyncWorldGenerator
	{
	public:
		/** @param workerCount Number of CPU terrain workers. */
		explicit AsyncWorldGenerator(ve::core::Index workerCount);

		/** @param request Chunk coordinate to generate. @return True when queued. */
		bool RequestChunk(ChunkGenerationRequest request);

		/** Queues a grid associated with one world chunk-storage epoch. */
		void RequestGrid(
			const FlatWorldSpawnSettings& settings,
			std::uint64_t chunkStorageRevision);

		/** @return Completed generated chunks ready for the game thread. */
		ve::core::DynamicArray<ChunkGenerationResult> DrainCompletedChunks();

		/** @return Number of terrain requests not yet drained by the game thread. */
		ve::core::Index PendingTaskCount() const;

	private:
		ve::network::ThreadSafeMessageQueue<ChunkGenerationResult> completedChunks_;
		std::atomic<ve::core::Index> outstandingRequestCount_ = 0;
		ve::tasks::BackgroundTaskQueue backgroundTaskQueue_;
	};
}
