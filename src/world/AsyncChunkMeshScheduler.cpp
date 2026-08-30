#include "AsyncChunkMeshScheduler.h"

#include "WorldGridMath.h"

#include <cstdint>
#include <exception>
#include <utility>

namespace ve::world::mesh
{
	namespace
	{
		std::uint64_t BuildCancellationIdForChunk(int chunkCoordinateX, int chunkCoordinateZ) noexcept
		{
			const std::uint64_t chunkCoordinateXBits = ve::world::grid::SignedCoordinateBits(chunkCoordinateX);
			const std::uint64_t chunkCoordinateZBits = ve::world::grid::SignedCoordinateBits(chunkCoordinateZ);
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
		try
		{
			_backgroundTasks.CancelPendingTasks(cancellationId);
			ve::tasks::BackgroundTaskOptions taskOptions;
			taskOptions.cancellationId = cancellationId;
			const int chunkX = request.chunk.chunkX;
			const int chunkZ = request.chunk.chunkZ;
			const std::uint64_t chunkRevision = request.chunk.revision;
			const std::uint64_t chunkStorageRevision = request.chunkStorageRevision;
			taskOptions.failureHandler = [this, chunkX, chunkZ, chunkRevision, chunkStorageRevision](std::exception_ptr)
			{
				try
				{
					_completedBuilds.Push(ChunkMeshBuildOutput{
						chunkX, chunkZ, chunkRevision, {}, false, chunkStorageRevision });
				}
				catch (...) { _buildFailureRecoveryRequired.store(true, std::memory_order_release); }
			};
			return _backgroundTasks.Enqueue([this, request = std::move(request), &blockRegistry]() mutable
			{
				_completedBuilds.Push(BuildChunkMeshOutput(request, blockRegistry));
			}, std::move(taskOptions));
		}
		catch (...) { return false; }
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

	bool AsyncChunkMeshScheduler::ConsumeBuildFailureRecoveryRequest() noexcept
	{
		return _buildFailureRecoveryRequired.exchange(false, std::memory_order_acq_rel);
	}
}
