#include "AsyncChunkMeshScheduler.h"

#include <utility>

namespace ve::world::mesh
{
	AsyncChunkMeshScheduler::AsyncChunkMeshScheduler(std::size_t workerCount)
		: _backgroundTasks(workerCount)
	{
	}

	bool AsyncChunkMeshScheduler::RequestBuild(ChunkMeshBuildRequest request, const ve::blocks::BlockRegistry& blockRegistry)
	{
		return _backgroundTasks.Enqueue([this, request = std::move(request), &blockRegistry]() mutable
		{
			_completedBuilds.Push(BuildChunkMeshOutput(request, blockRegistry));
		});
	}

	std::vector<ChunkMeshBuildOutput> AsyncChunkMeshScheduler::DrainCompletedBuilds()
	{
		return _completedBuilds.Drain();
	}

	std::size_t AsyncChunkMeshScheduler::PendingTaskCount() const
	{
		return _backgroundTasks.PendingTaskCount();
	}
}
