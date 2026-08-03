	{
		ResetChunkMeshCacheForWorldStorage(world);
		const ve::world::WorldMetrics metrics = world.Metrics();
		const int world_width_blocks = metrics.worldSizeInChunks * Chunk::CHUNK_WIDTH;
		const int world_depth_blocks = metrics.worldSizeInChunks * Chunk::CHUNK_DEPTH;
		vertices.clear();
		indices.clear();
		last_rebuilt_chunk_count_ = 0;
		const std::size_t world_column_count = WorldColumnCount(world_width_blocks, world_depth_blocks);
		vertices.reserve(std::max<std::size_t>(140'000u, world_column_count * 16u));
		indices.reserve(std::max<std::size_t>(210'000u, world_column_count * 24u));

		for (const Chunk& chunk : world.Chunks())
		{
			CachedChunkMesh& cached_mesh = CachedMeshFor(chunk);
			if (cached_mesh.mesh_revision != chunk.MeshRevision())
			{
				RebuildChunkMesh(world, block_registry, chunk, cached_mesh);
				++last_rebuilt_chunk_count_;
			}
			AppendCachedChunkMesh(cached_mesh, vertices, indices);
		}
	}

	void VulkanGpuChunkRenderer::AppendVisibleBlockFaces(const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		int block_x,
		int block_y,
		int block_z,
		ve::blocks::BlockId block,
		std::vector<VoxelVertex>& vertices,
		std::vector<std::uint32_t>& indices) const
	{
		for (const BlockFaceGeometry& face : ChunkFaces())
		{
			const ve::blocks::BlockId neighbor_block = world.GetBlock(
				block_x + face.neighbor_offset.x,
				block_y + face.neighbor_offset.y,
				block_z + face.neighbor_offset.z);
			if (!OccludesNeighborFaces(block_registry, neighbor_block))
			{
				AppendFaceMesh(face, world, block_registry, block_x, block_y, block_z, block, vertices, indices);
			}
		}
	}
}
