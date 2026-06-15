#include "AsyncChunkMeshScheduler.h"

#include <utility>

namespace ve::world::mesh
{
	/// Starts worker threads used for CPU chunk mesh builds.
	AsyncChunkMeshScheduler::AsyncChunkMeshScheduler(std::size_t workerCount)
		: _backgroundTasks(workerCount)
	{
	}

	/// Schedules one snapshot-backed mesh build.
	bool AsyncChunkMeshScheduler::RequestBuild(ChunkMeshBuildRequest request, const ve::blocks::BlockRegistry& blockRegistry)
	{
		return _backgroundTasks.Enqueue([this, request = std::move(request), &blockRegistry]() mutable
		{
			_completedBuilds.Push(BuildChunkMeshOutput(request, blockRegistry));
		});
	}

	/// Returns completed mesh builds without blocking the game thread.
	std::vector<ChunkMeshBuildOutput> AsyncChunkMeshScheduler::DrainCompletedBuilds()
	{
		return _completedBuilds.Drain();
	}

	/// Reports CPU mesh tasks waiting to start.
	std::size_t AsyncChunkMeshScheduler::PendingTaskCount() const
	{
		return _backgroundTasks.PendingTaskCount();
	}
}
