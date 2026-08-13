#pragma once

#include <cstddef>

namespace ve::world
{
	enum class ChunkStoragePolicy
	{
		FixedReserve,
		GrowOnDemand
	};

	struct WorldCreateInfo
	{
		std::size_t chunkCapacity = 0;
		ChunkStoragePolicy chunkStoragePolicy = ChunkStoragePolicy::FixedReserve;
	};

	struct WorldMetrics
	{
		int worldSizeInChunks;
		std::size_t loadedChunkCount;
		std::size_t reservedChunkCapacity;
		std::size_t chunksNeedingMeshBuild;
		std::size_t chunksWithQueuedMeshBuild;
		std::size_t pendingChunkMeshTaskCount;
		std::size_t pendingChunkMeshUploadCount;
		std::size_t pendingWorldGenerationTaskCount;
		std::size_t reservedChunkStorageBytes;
		std::size_t levelArenaCapacityBytes;
		std::size_t levelArenaUsedBytes;
		std::size_t pendingWorldEventCount;
		ChunkStoragePolicy chunkStoragePolicy;
	};
}
