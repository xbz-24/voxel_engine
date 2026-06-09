#include "ChunkMeshCollector.h"

#include "GreedyChunkMesher.h"

namespace ve::world::mesh
{
	void CollectChunkFaces(const ChunkMeshInput& meshInput, const ve::blocks::BlockRegistry& blockRegistry, const NeighborMeshInputs& neighbors, std::vector<MeshFace>& faces)
	{
		GreedyChunkMesher{ meshInput, blockRegistry, neighbors }.CollectFaces(faces);
	}
}
