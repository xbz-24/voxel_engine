#pragma once

#include "BlockRegistry.h"
#include "ChunkMeshInput.h"
#include "ChunkMeshTypes.h"

#include <vector>

namespace ve::world::mesh
{
	/**
	 * Collects visible block faces from a chunk without issuing OpenGL calls.
	 *
	 * @param meshInput Chunk coordinate and block view inspected by the collector.
	 * @param blockRegistry Registry used for solidity and textures.
	 * @param neighbors Neighbor block views used for border culling.
	 * @param faces Destination face list.
	 */
	void CollectChunkFaces(const ChunkMeshInput& meshInput, const ve::blocks::BlockRegistry& blockRegistry, const NeighborMeshInputs& neighbors, std::vector<MeshFace>& faces);
}
