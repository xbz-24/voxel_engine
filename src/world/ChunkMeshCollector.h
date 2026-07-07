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
	 * @param mesh_input Chunk coordinate and block view inspected by the collector.
	 * @param block_registry Registry used for solidity and textures.
	 * @param neighbors Neighbor block views used for border culling.
	 * @param faces Destination face list.
	 * @param diagnostics Optional counters filled while collecting faces.
	 */
	void CollectChunkFaces(
		const ChunkMeshInput& mesh_input,
		const ve::blocks::BlockRegistry& block_registry,
		const NeighborMeshInputs& neighbors,
		std::vector<MeshFace>& faces,
		ChunkMeshBuildDiagnostics* diagnostics = nullptr);
}
