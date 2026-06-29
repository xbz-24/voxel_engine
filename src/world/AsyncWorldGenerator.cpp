#include "AsyncWorldGenerator.h"

namespace ve::world::generation
{
	namespace
	{
		/// Copies generated chunk storage into a flat result buffer.
		void CopyStorage(const terrain::BlockStorage& storage, ChunkGenerationResult& result)
		{
			ve::core::Index destinationBlockIndex = 0;
			for (int localBlockX = 0; localBlockX < terrain::ChunkWidth; localBlockX++)
				for (int localBlockY = 0; localBlockY < terrain::ChunkHeight; localBlockY++)
					for (int localBlockZ = 0; localBlockZ < terrain::ChunkDepth; localBlockZ++)
						result.blocks[destinationBlockIndex++] = storage[localBlockX][localBlockY][localBlockZ];
		}

		/// Generates one chunk result completely on a worker thread.
		ChunkGenerationResult GenerateChunk(ChunkGenerationRequest request)
		{
			terrain::BlockStorage storage{};
			terrain::GenerateChunkTerrain(request.chunk_x, request.chunk_z, storage);
			ChunkGenerationResult result{ request.chunk_x, request.chunk_z, {} };
			CopyStorage(storage, result);
			return result;
		}
	}

	/// Starts the async terrain generator.
	AsyncWorldGenerator::AsyncWorldGenerator(ve::core::Index workerCount)
		: backgroundTaskQueue_(workerCount)
	{
	}

	/// Queues one chunk coordinate for background generation.
	bool AsyncWorldGenerator::RequestChunk(ChunkGenerationRequest request)
	{
		outstandingRequestCount_.fetch_add(1, std::memory_order_relaxed);
		if (!backgroundTaskQueue_.Enqueue([this, request]()
		{
			completedChunks_.Push(GenerateChunk(request));
		}))
		{
			outstandingRequestCount_.fetch_sub(1, std::memory_order_relaxed);
			return false;
		}
		return true;
	}

	/// Queues every chunk in a square world for background generation.
	void AsyncWorldGenerator::RequestGrid(const FlatWorldSpawnSettings& settings)
	{
		for (int chunkX = 0; chunkX < settings.worldSizeChunks; chunkX++)
			for (int chunkZ = 0; chunkZ < settings.worldSizeChunks; chunkZ++)
				RequestChunk(ChunkGenerationRequest{ chunkX, chunkZ });
	}

	/// Drains completed generated chunks without blocking the game thread.
	ve::core::DynamicArray<ChunkGenerationResult> AsyncWorldGenerator::DrainCompletedChunks()
	{
		ve::core::DynamicArray<ChunkGenerationResult> completedGeneratedChunks = completedChunks_.Drain();
		if (!completedGeneratedChunks.empty())
		{
			outstandingRequestCount_.fetch_sub(completedGeneratedChunks.size(), std::memory_order_relaxed);
		}
		return completedGeneratedChunks;
	}

	/// Reports terrain requests that still need to be drained on the game thread.
	ve::core::Index AsyncWorldGenerator::PendingTaskCount() const
	{
		return outstandingRequestCount_.load(std::memory_order_relaxed);
	}
}
