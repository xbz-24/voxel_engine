#pragma once

#include "BackgroundTaskQueue.h"
#include "ChunkTerrain.h"
#include "CoreTypes.h"
#include "ThreadSafeMessageQueue.h"
#include "WorldConfiguration.h"

#include <atomic>

namespace ve::world::generation
{
	struct ChunkGenerationRequest
	{
		int chunkCoordinateX = 0;
		int chunkCoordinateZ = 0;
	};

	struct ChunkGenerationResult
	{
		int chunkCoordinateX = 0;
		int chunkCoordinateZ = 0;
		ve::core::StaticArray<ve::blocks::BlockId, terrain::ChunkBlockCount> blocks{};
	};

	class AsyncWorldGenerator
	{
	public:
		/** @param workerCount Number of CPU terrain workers. */
		explicit AsyncWorldGenerator(ve::core::Index workerCount);

		/** @param request Chunk coordinate to generate. @return True when queued. */
		bool RequestChunk(ChunkGenerationRequest request);

		/** @param settings Square world settings used to queue every chunk. */
		void RequestGrid(const FlatWorldSpawnSettings& settings);

		/** @return Completed generated chunks ready for the game thread. */
		ve::core::DynamicArray<ChunkGenerationResult> DrainCompletedChunks();

		/** @return Number of terrain requests not yet drained by the game thread. */
		ve::core::Index PendingTaskCount() const;

	private:
		ve::network::ThreadSafeMessageQueue<ChunkGenerationResult> completedChunks_;
		ve::tasks::BackgroundTaskQueue backgroundTaskQueue_;
		std::atomic<ve::core::Index> outstandingRequestCount_ = 0;
	};
}
