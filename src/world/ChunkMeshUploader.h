#pragma once

#include "ChunkMeshRequest.h"

class Chunk;

namespace ve::world::mesh
{
	/**
	 * Uploads a completed mesh output to its matching chunk.
	 *
	 * @param chunk Chunk that should receive the GPU mesh.
	 * @param output CPU mesh output produced by the async builder.
	 * @return True when chunk coordinates matched and upload was performed.
	 */
	bool TryUploadChunkMeshOutput(Chunk& chunk, ChunkMeshBuildOutput output);
}
