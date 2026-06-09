#include "ChunkMeshUploadQueue.h"

#include <utility>

namespace ve::world::mesh
{
	/// Adds one completed CPU mesh to the upload backlog.
	void ChunkMeshUploadQueue::Push(ChunkMeshBuildOutput output)
	{
		_pendingUploads.push_back(std::move(output));
	}

	/// Adds many completed CPU meshes to the upload backlog.
	void ChunkMeshUploadQueue::PushAll(std::vector<ChunkMeshBuildOutput> outputs)
	{
		_pendingUploads.reserve(_pendingUploads.size() + outputs.size());
		for (ChunkMeshBuildOutput& output : outputs)
		{
			Push(std::move(output));
		}
	}

	/// Moves all pending upload work to the caller.
	std::vector<ChunkMeshBuildOutput> ChunkMeshUploadQueue::Drain()
	{
		std::vector<ChunkMeshBuildOutput> drainedUploads;
		drainedUploads.swap(_pendingUploads);
		return drainedUploads;
	}

	/// Reports upload work waiting for the main thread.
	std::size_t ChunkMeshUploadQueue::PendingUploadCount() const noexcept
	{
		return _pendingUploads.size();
	}
}
