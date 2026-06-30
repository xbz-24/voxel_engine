#pragma once

#include "BlockRegistry.h"
#include "ChunkMeshInput.h"
#include "ChunkMeshTypes.h"

namespace ve::world::mesh
{
	/**
	 * Builds CPU vertex/batch data for one chunk mesh.
	 *
	 * @param chunk Chunk whose blocks are converted to renderable geometry.
	 * @param block_registry Registry used for block textures and solidity.
	 * @param neighbors Neighbor chunk pointers used to cull border faces.
	 * @return Vertices, texture batches, and build diagnostics ready for GPU upload.
	 */
	ChunkMeshBuildResult BuildChunkMesh(const Chunk& chunk, const ve::blocks::BlockRegistry& block_registry, const NeighborChunks& neighbors);

	/**
	 * Builds CPU vertex/batch data from a chunk mesh input.
	 *
	 * @param mesh_input Chunk coordinate and block storage view.
	 * @param block_registry Registry used for block textures and solidity.
	 * @param neighbors Neighbor block views used to cull border faces.
	 * @return Vertices, texture batches, and build diagnostics ready for GPU upload.
	 */
	ChunkMeshBuildResult BuildChunkMesh(const ChunkMeshInput& mesh_input, const ve::blocks::BlockRegistry& block_registry, const NeighborMeshInputs& neighbors);
}
