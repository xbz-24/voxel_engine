#include "ChunkMeshUploader.h"

#include "Chunk.h"

#include <utility>

namespace ve::world::mesh
{
	/// Uploads a completed mesh output to its matching chunk.
	bool TryUploadChunkMeshOutput(Chunk& chunk, ChunkMeshBuildOutput output)
	{
		if (chunk.GetChunkX() != output.chunkX || chunk.GetChunkZ() != output.chunkZ)
		{
			return false;
		}

		chunk.UploadMesh(std::move(output.mesh));
		return true;
	}
}
