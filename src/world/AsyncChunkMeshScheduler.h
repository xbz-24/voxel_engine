#pragma once

#include "BackgroundTaskQueue.h"
#include "ChunkMeshRequest.h"
#include "ThreadSafeMessageQueue.h"

#include <cstddef>
#include <vector>

namespace ve::world::mesh
{
	class AsyncChunkMeshScheduler
	{
	public:
		/**
		 * Starts worker threads used for CPU chunk mesh builds.
		 *
		 * @param workerCount Number of mesh worker threads.
		 */
		explicit AsyncChunkMeshScheduler(std::size_t workerCount);

		/**
		 * Schedules one snapshot-backed mesh build.
		 *
		 * @param request Owned chunk snapshots safe for background work.
		 * @param blockRegistry Immutable block registry; must outlive the task.
		 * @return True when the task was accepted.
		 */
		bool RequestBuild(ChunkMeshBuildRequest request, const ve::blocks::BlockRegistry& blockRegistry);

		/**
		 * Returns completed mesh builds without blocking the game thread.
		 *
		 * @return Mesh outputs accumulated by worker threads.
		 */
		std::vector<ChunkMeshBuildOutput> DrainCompletedBuilds();

		/**
		 * Reports CPU mesh tasks waiting to start.
		 *
		 * @return Number of queued background tasks.
		 */
		std::size_t PendingTaskCount() const;

	private:
		ve::network::ThreadSafeMessageQueue<ChunkMeshBuildOutput> _completedBuilds;
		ve::tasks::BackgroundTaskQueue _backgroundTasks;
	};
}
