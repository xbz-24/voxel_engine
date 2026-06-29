#include "AsyncWorldGenerator.h"

namespace ve::world::generation
{
	namespace
	{
		/// Copies generated chunk storage into a flat result buffer.
		void CopyStorage(const terrain::BlockStorage& storage, ChunkGenerationResult& result)
		{
			ve::core::Index write_index = 0;
			for (int local_block_x = 0; local_block_x < terrain::ChunkWidth; local_block_x++)
				for (int local_block_y = 0; local_block_y < terrain::ChunkHeight; local_block_y++)
					for (int local_block_z = 0; local_block_z < terrain::ChunkDepth; local_block_z++)
						result.blocks[write_index++] = storage[local_block_x][local_block_y][local_block_z];
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
	AsyncWorldGenerator::AsyncWorldGenerator(ve::core::Index worker_count)
		: background_tasks_(worker_count)
	{
	}

	/// Queues one chunk coordinate for background generation.
	bool AsyncWorldGenerator::RequestChunk(ChunkGenerationRequest request)
	{
		outstanding_requests_.fetch_add(1, std::memory_order_relaxed);
		if (!background_tasks_.Enqueue([this, request]()
		{
			completed_chunks_.Push(GenerateChunk(request));
		}))
		{
			outstanding_requests_.fetch_sub(1, std::memory_order_relaxed);
			return false;
		}
		return true;
	}

	/// Queues every chunk in a square world for background generation.
	void AsyncWorldGenerator::RequestGrid(const FlatWorldSpawnSettings& settings)
	{
		for (int chunk_x = 0; chunk_x < settings.worldSizeChunks; chunk_x++)
			for (int chunk_z = 0; chunk_z < settings.worldSizeChunks; chunk_z++)
				RequestChunk(ChunkGenerationRequest{ chunk_x, chunk_z });
	}

	/// Drains completed generated chunks without blocking the game thread.
	ve::core::DynamicArray<ChunkGenerationResult> AsyncWorldGenerator::DrainCompletedChunks()
	{
		ve::core::DynamicArray<ChunkGenerationResult> completed_chunks = completed_chunks_.Drain();
		if (!completed_chunks.empty())
		{
			outstanding_requests_.fetch_sub(completed_chunks.size(), std::memory_order_relaxed);
		}
		return completed_chunks;
	}

	/// Reports terrain requests that still need to be drained on the game thread.
	ve::core::Index AsyncWorldGenerator::PendingTaskCount() const
	{
		return outstanding_requests_.load(std::memory_order_relaxed);
	}
}
