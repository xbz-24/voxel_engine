#pragma once

#include "BlockRegistry.h"
#include "ChunkMeshTypes.h"

namespace ve::world::mesh
{
	/**
	 * Builds CPU vertex/batch data for one chunk mesh.
	 *
	 * @param chunk Chunk whose blocks are converted to renderable geometry.
	 * @param blockRegistry Registry used for block textures and solidity.
	 * @param neighbors Neighbor chunk pointers used to cull border faces.
	 * @return Vertices and texture batches ready for GPU upload.
	 */
	ChunkMeshBuildResult BuildChunkMesh(const Chunk& chunk, const ve::blocks::BlockRegistry& blockRegistry, const NeighborChunks& neighbors);
}
