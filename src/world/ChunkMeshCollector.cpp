#include "ChunkMeshCollector.h"

#include "GreedyChunkMesher.h"

namespace ve::world::mesh
{
	void CollectChunkFaces(
		const ChunkMeshInput& mesh_input,
		const ve::blocks::BlockRegistry& block_registry,
		const NeighborMeshInputs& neighbors,
		std::vector<MeshFace>& faces,
		ChunkMeshBuildDiagnostics* diagnostics)
	{
		GreedyChunkMesher{ mesh_input, block_registry, neighbors }.CollectFaces(faces, diagnostics);
	}
}
