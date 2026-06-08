#pragma once

#include "BlockRegistry.h"
#include "ChunkMeshTypes.h"

#include <vector>

namespace ve::world::mesh
{
	/**
	 * Collects visible block faces from a chunk without issuing OpenGL calls.
	 *
	 * @param chunk Chunk whose CPU block data is inspected.
	 * @param blockRegistry Registry used for solidity and textures.
	 * @param neighbors Loaded neighbor chunks used for border culling.
	 * @param faces Destination face list.
	 */
	void CollectChunkFaces(const Chunk& chunk, const ve::blocks::BlockRegistry& blockRegistry, const NeighborChunks& neighbors, std::vector<MeshFace>& faces);
}
