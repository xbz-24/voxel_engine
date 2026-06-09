#include "ChunkMeshRequest.h"

#include "ChunkMeshBuilder.h"

namespace ve::world::mesh
{
	ChunkMeshBuildOutput BuildChunkMeshOutput(const ChunkMeshBuildRequest& request, const ve::blocks::BlockRegistry& blockRegistry)
	{
		const ChunkMeshInput chunkInput = request.chunk.CreateInput();
		const ChunkMeshInput west = request.west ? request.west->CreateInput() : ChunkMeshInput{ 0, 0, {} };
		const ChunkMeshInput east = request.east ? request.east->CreateInput() : ChunkMeshInput{ 0, 0, {} };
		const ChunkMeshInput north = request.north ? request.north->CreateInput() : ChunkMeshInput{ 0, 0, {} };
		const ChunkMeshInput south = request.south ? request.south->CreateInput() : ChunkMeshInput{ 0, 0, {} };
		const NeighborMeshInputs neighbors{
			request.west ? &west : nullptr,
			request.east ? &east : nullptr,
			request.north ? &north : nullptr,
			request.south ? &south : nullptr
		};
		return ChunkMeshBuildOutput{
			chunkInput.chunkX,
			chunkInput.chunkZ,
			BuildChunkMesh(chunkInput, blockRegistry, neighbors)
		};
	}
}
