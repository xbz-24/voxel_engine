#pragma once

#include "AsyncChunkMeshScheduler.h"
#include "ChunkMeshUploadQueue.h"

#include <cstddef>

namespace ve::world::mesh
{
	struct ChunkMeshPipelineStats
	{
		std::size_t pendingBuildTaskCount = 0;
		std::size_t pendingUploadCount = 0;
	};

	class ChunkMeshPipeline
	{
	public:
		/**
		 * Creates the async mesh pipeline.
		 *
		 * @param workerCount Number of CPU mesh worker threads.
		 */
		explicit ChunkMeshPipeline(std::size_t workerCount);

		/**
		 * Schedules one chunk mesh build from owned snapshots.
		 *
		 * @param request Snapshot-backed mesh build request.
		 * @param blockRegistry Immutable block registry used during CPU build.
		 * @return True when the request was accepted.
		 */
		bool RequestBuild(ChunkMeshBuildRequest request, const ve::blocks::BlockRegistry& blockRegistry);

		/**
		 * Moves completed worker results into the upload backlog.
		 */
		void CollectCompletedBuilds();

		/**
		 * Moves upload-ready meshes to the caller.
		 *
		 * @return Mesh outputs ready for main-thread GPU upload.
		 */
		std::vector<ChunkMeshBuildOutput> DrainUploadBacklog();

		/** @return CPU mesh tasks waiting to start. */
		std::size_t PendingTaskCount() const;

		/** @return Mesh outputs waiting for GPU upload. */
		std::size_t PendingUploadCount() const;

		/** @return Snapshot of async build and upload backlog counters. */
		ChunkMeshPipelineStats Stats() const;

	private:
		AsyncChunkMeshScheduler _scheduler;
		ChunkMeshUploadQueue _uploadQueue;
	};
}
