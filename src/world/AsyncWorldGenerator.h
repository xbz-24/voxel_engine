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
		int chunk_x = 0;
		int chunk_z = 0;
	};

	struct ChunkGenerationResult
	{
		int chunk_x = 0;
		int chunk_z = 0;
		ve::core::StaticArray<ve::blocks::BlockId, terrain::ChunkBlockCount> blocks{};
	};

	class AsyncWorldGenerator
	{
	public:
		/** @param worker_count Number of CPU terrain workers. */
		explicit AsyncWorldGenerator(ve::core::Index worker_count);

		/** @param request Chunk coordinate to generate. @return True when queued. */
		bool RequestChunk(ChunkGenerationRequest request);

		/** @param settings Square world settings used to queue every chunk. */
		void RequestGrid(const FlatWorldSpawnSettings& settings);

		/** @return Completed generated chunks ready for the game thread. */
		ve::core::DynamicArray<ChunkGenerationResult> DrainCompletedChunks();

		/** @return Number of terrain requests not yet drained by the game thread. */
		ve::core::Index PendingTaskCount() const;

	private:
		ve::network::ThreadSafeMessageQueue<ChunkGenerationResult> completed_chunks_;
		ve::tasks::BackgroundTaskQueue background_tasks_;
		std::atomic<ve::core::Index> outstanding_requests_ = 0;
	};
}
