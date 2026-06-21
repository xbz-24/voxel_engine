#pragma once

#include "ChunkMeshRequest.h"

#include <cstddef>
#include <vector>

namespace ve::world::mesh
{
	class ChunkMeshUploadQueue
	{
	public:
		/**
		 * Adds one completed CPU mesh to the upload backlog.
		 *
		 * @param output Mesh output moved into the queue.
		 */
		void Push(ChunkMeshBuildOutput output);

		/**
		 * Adds many completed CPU meshes to the upload backlog.
		 *
		 * @param outputs Mesh outputs moved into the queue.
		 */
		void PushAll(std::vector<ChunkMeshBuildOutput> outputs);

		/**
		 * Moves all pending upload work to the caller.
		 *
		 * @return Mesh outputs ready for main-thread GPU upload.
		 */
		std::vector<ChunkMeshBuildOutput> Drain();

		/**
		 * Reports upload work waiting for the main thread.
		 *
		 * @return Number of queued mesh uploads.
		 */
		std::size_t PendingUploadCount() const noexcept;

	private:
		std::vector<ChunkMeshBuildOutput> _pendingUploads;
	};
}
