#include "World.h"

#include "ChunkMeshUploader.h"

#include <utility>

namespace ve::world
{
	/**
	 * Captures one chunk and its loaded neighbors for async CPU meshing.
	 *
	 * @param chunkX Chunk-grid X coordinate.
	 * @param chunkZ Chunk-grid Z coordinate.
	 * @return Snapshot request, or empty when the chunk is missing.
	 */
	std::optional<ve::world::mesh::ChunkMeshBuildRequest> World::CaptureChunkMeshBuildRequest(int chunkX, int chunkZ) const
	{
		const Chunk* chunk = FindChunk(chunkX, chunkZ);
		if (!chunk)
		{
			return std::nullopt;
		}

		return ve::world::mesh::CaptureChunkMeshBuildRequest(*chunk, FindNeighborChunks(chunkX, chunkZ));
	}

	/**
	 * Uploads completed CPU mesh data to the matching loaded chunk.
	 *
	 * @param output Mesh output produced by a mesh worker.
	 * @return True when a matching chunk received the output.
	 */
	bool World::TryUploadChunkMeshOutput(ve::world::mesh::ChunkMeshBuildOutput output)
	{
		Chunk* chunk = FindChunk(output.chunkX, output.chunkZ);
		if (!chunk)
		{
			return false;
		}

		return ve::world::mesh::TryUploadChunkMeshOutput(*chunk, std::move(output));
	}
}
