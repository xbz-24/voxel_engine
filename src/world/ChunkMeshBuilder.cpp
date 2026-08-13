#include "ChunkMeshBuilder.h"

#include "Chunk.h"
#include "ChunkMeshAssembler.h"
#include "ChunkMeshCollector.h"

#include <optional>
#include <utility>

namespace ve::world::mesh
{
	ChunkMeshBuildResult BuildChunkMesh(
		const ChunkMeshInput& mesh_input,
		const ve::blocks::BlockRegistry& block_registry,
		const NeighborMeshInputs& neighbors)
	{
		std::vector<MeshFace> faces;
		ChunkMeshBuildDiagnostics diagnostics;
		CollectChunkFaces(mesh_input, block_registry, neighbors, faces, &diagnostics);
		return detail::AssembleChunkMesh(std::move(faces), diagnostics);
	}

	ChunkMeshBuildResult BuildChunkMesh(
		const Chunk& chunk,
		const ve::blocks::BlockRegistry& block_registry,
		const NeighborChunks& neighbors)
	{
		const ChunkMeshInput chunk_mesh_input = chunk.CreateMeshInput();
		const std::optional<ChunkMeshInput> west =
			neighbors.west ? std::optional(neighbors.west->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> east =
			neighbors.east ? std::optional(neighbors.east->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> north =
			neighbors.north ? std::optional(neighbors.north->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> south =
			neighbors.south ? std::optional(neighbors.south->CreateMeshInput()) : std::nullopt;
		return BuildChunkMesh(chunk_mesh_input, block_registry, NeighborMeshInputs{
			west ? &*west : nullptr,
			east ? &*east : nullptr,
			north ? &*north : nullptr,
			south ? &*south : nullptr
		});
	}
}
