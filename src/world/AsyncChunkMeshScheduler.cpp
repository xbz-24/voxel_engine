#include "AsyncChunkMeshScheduler.h"

#include <cstdint>
#include <utility>

namespace ve::world::mesh
{
	namespace
	{
		std::uint64_t BuildCancellationIdForChunk(int chunkCoordinateX, int chunkCoordinateZ) noexcept
		{
			const std::uint64_t chunkCoordinateXBits = static_cast<std::uint32_t>(chunkCoordinateX);
			const std::uint64_t chunkCoordinateZBits = static_cast<std::uint32_t>(chunkCoordinateZ);
			return 0x9e3779b97f4a7c15ULL ^ (chunkCoordinateXBits << 32U) ^ chunkCoordinateZBits;
		}
	}

	/// Starts worker threads used for CPU chunk mesh builds.
	AsyncChunkMeshScheduler::AsyncChunkMeshScheduler(std::size_t workerCount)
		: _backgroundTasks(workerCount)
	{
	}

	/// Schedules one snapshot-backed mesh build.
	bool AsyncChunkMeshScheduler::RequestBuild(ChunkMeshBuildRequest request, const ve::blocks::BlockRegistry& blockRegistry)
	{
		const std::uint64_t cancellationId = BuildCancellationIdForChunk(request.chunk.chunkX, request.chunk.chunkZ);
		_backgroundTasks.CancelPendingTasks(cancellationId);
		ve::tasks::BackgroundTaskOptions taskOptions;
		taskOptions.cancellationId = cancellationId;
		return _backgroundTasks.Enqueue([this, request = std::move(request), &blockRegistry]() mutable
		{
			_completedBuilds.Push(BuildChunkMeshOutput(request, blockRegistry));
		}, taskOptions);
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
