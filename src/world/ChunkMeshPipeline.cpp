#include "ChunkMeshPipeline.h"

#include <utility>

namespace ve::world::mesh
{
	/// Creates the async mesh pipeline.
	ChunkMeshPipeline::ChunkMeshPipeline(std::size_t workerCount)
		: _scheduler(workerCount)
	{
	}

	/// Schedules one chunk mesh build from owned snapshots.
	bool ChunkMeshPipeline::RequestBuild(ChunkMeshBuildRequest request, const ve::blocks::BlockRegistry& blockRegistry)
	{
		return _scheduler.RequestBuild(std::move(request), blockRegistry);
	}

	/// Moves completed worker results into the upload backlog.
	void ChunkMeshPipeline::CollectCompletedBuilds()
	{
		_uploadQueue.PushAll(_scheduler.DrainCompletedBuilds());
	}

	/// Moves upload-ready meshes to the caller.
	std::vector<ChunkMeshBuildOutput> ChunkMeshPipeline::DrainUploadBacklog()
	{
		return _uploadQueue.Drain();
	}

	/// Reports CPU mesh tasks waiting to start.
	std::size_t ChunkMeshPipeline::PendingTaskCount() const
	{
		return _scheduler.PendingTaskCount();
	}

	/// Reports mesh outputs waiting for GPU upload.
	std::size_t ChunkMeshPipeline::PendingUploadCount() const
	{
		return _uploadQueue.PendingUploadCount();
	}

	/// Reports async build and upload backlog counters.
	ChunkMeshPipelineStats ChunkMeshPipeline::Stats() const
	{
		return ChunkMeshPipelineStats{
			PendingTaskCount(),
			PendingUploadCount()
		};
	}
}
