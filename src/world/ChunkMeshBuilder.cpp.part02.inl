		CollectChunkFaces(mesh_input, block_registry, neighbors, faces, &diagnostics);
		ChunkMeshAssembler chunk_mesh_assembler(std::move(faces), diagnostics);
		return chunk_mesh_assembler.Build();
	}

	ChunkMeshBuildResult BuildChunkMesh(
		const Chunk& chunk,
		const ve::blocks::BlockRegistry& block_registry,
		const NeighborChunks& neighbors)
	{
		const ChunkMeshInput chunk_mesh_input = chunk.CreateMeshInput();
		const std::optional<ChunkMeshInput> west_neighbor_input =
			neighbors.west ? std::optional(neighbors.west->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> east_neighbor_input =
			neighbors.east ? std::optional(neighbors.east->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> north_neighbor_input =
			neighbors.north ? std::optional(neighbors.north->CreateMeshInput()) : std::nullopt;
		const std::optional<ChunkMeshInput> south_neighbor_input =
			neighbors.south ? std::optional(neighbors.south->CreateMeshInput()) : std::nullopt;
		return BuildChunkMesh(chunk_mesh_input, block_registry, NeighborMeshInputs{
			west_neighbor_input ? &*west_neighbor_input : nullptr,
			east_neighbor_input ? &*east_neighbor_input : nullptr,
			north_neighbor_input ? &*north_neighbor_input : nullptr,
			south_neighbor_input ? &*south_neighbor_input : nullptr
		});
	}
}
