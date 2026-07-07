#include "ChunkMeshRequest.h"

#include "ChunkMeshBuilder.h"

namespace ve::world::mesh
{
	/// Captures a snapshot-backed build request from loaded chunks.
	ChunkMeshBuildRequest CaptureChunkMeshBuildRequest(const Chunk& chunk, const NeighborChunks& neighbors)
	{
		return ChunkMeshBuildRequest{
			CaptureChunkMeshSnapshot(chunk),
			neighbors.west ? std::optional(CaptureChunkMeshSnapshot(*neighbors.west)) : std::nullopt,
			neighbors.east ? std::optional(CaptureChunkMeshSnapshot(*neighbors.east)) : std::nullopt,
			neighbors.north ? std::optional(CaptureChunkMeshSnapshot(*neighbors.north)) : std::nullopt,
			neighbors.south ? std::optional(CaptureChunkMeshSnapshot(*neighbors.south)) : std::nullopt
		};
	}

	/// Builds CPU mesh output from owned chunk snapshots.
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
			request.chunk.revision,
			BuildChunkMesh(chunkInput, blockRegistry, neighbors)
		};
	}
}
