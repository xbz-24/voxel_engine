namespace
{
	std::size_t LocalBlockIndex(int localBlockX, int localBlockY, int localBlockZ)
	{
		return static_cast<std::size_t>(
			(localBlockX * ve::world::terrain::ChunkHeight + localBlockY) *
			ve::world::terrain::ChunkDepth + localBlockZ);
	}

	void SetLocalBlock(
		std::vector<ve::blocks::BlockId>& chunkBlocks,
		int localBlockX,
		int localBlockY,
		int localBlockZ,
		ve::blocks::BlockId blockId)
	{
		chunkBlocks[LocalBlockIndex(localBlockX, localBlockY, localBlockZ)] = blockId;
	}

}

TEST_CASE("chunk mesh diagnostics count visible and culled block faces")
{
	std::vector<ve::blocks::BlockId> blocks(ve::world::mesh::ChunkBlockCount, ve::blocks::BlockId::Air);
	SetLocalBlock(blocks, 0, 0, 0, ve::blocks::BlockId::Dirt);
	SetLocalBlock(blocks, 1, 0, 0, ve::blocks::BlockId::Dirt);

	const ve::assets::AssetPaths assetPaths{};
	const ve::blocks::BlockRegistry blockRegistry(
		assetPaths,
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);
	const ve::world::mesh::ChunkMeshInput input{ 0, 0, blocks };
	const ve::world::mesh::NeighborMeshInputs neighbors{};

	const ve::world::mesh::ChunkMeshBuildResult mesh = ve::world::mesh::BuildChunkMesh(input, blockRegistry, neighbors);

	CHECK(mesh.diagnostics.candidate_block_face_count == 12U);
	CHECK(mesh.diagnostics.visible_block_face_count == 10U);
	CHECK(mesh.diagnostics.culled_block_face_count == 2U);
	CHECK(mesh.diagnostics.merged_face_count == 6U);
	CHECK(mesh.diagnostics.vertex_count == mesh.vertices.size());
	CHECK(mesh.diagnostics.batch_count == mesh.batches.size());
	CHECK(mesh.diagnostics.MergeRatio() == doctest::Approx(10.0f / 6.0f));
}

TEST_CASE("world metrics include dirty chunks and reserved chunk bytes")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(2));

	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 2 });
	const ve::world::WorldMetrics metrics = world.Metrics();

	CHECK(metrics.worldSizeInChunks == 2);
	CHECK(metrics.loadedChunkCount == 4U);
	CHECK(metrics.reservedChunkCapacity == 4U);
	CHECK(metrics.chunksNeedingMeshBuild == 4U);
	CHECK(metrics.chunksWithQueuedMeshBuild == 0U);
	CHECK(metrics.pendingChunkMeshTaskCount == 0U);
	CHECK(metrics.pendingChunkMeshUploadCount == 0U);
	CHECK(metrics.pendingWorldGenerationTaskCount == 0U);
	CHECK(metrics.reservedChunkStorageBytes >= metrics.reservedChunkCapacity * sizeof(::Chunk));
	CHECK(metrics.levelArenaCapacityBytes >= metrics.levelArenaUsedBytes);
	CHECK(metrics.levelArenaUsedBytes >= metrics.reservedChunkStorageBytes);
	CHECK(metrics.pendingWorldEventCount == 4U);
}

TEST_CASE("world respawn clears stale pending events")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(2));

	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 2 });
	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	std::vector<ve::world::WorldEvent> generatedEventsAfterRespawn = world.DrainEvents();

	CHECK(generatedEventsAfterRespawn.size() == 1U);
	REQUIRE(!generatedEventsAfterRespawn.empty());
	CHECK(generatedEventsAfterRespawn.front().eventType == ve::world::WorldEventType::ChunkGenerated);
	CHECK(generatedEventsAfterRespawn.front().chunkGenerated.chunkCoordinateX == 0);
	CHECK(generatedEventsAfterRespawn.front().chunkGenerated.chunkCoordinateZ == 0);

	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	CHECK(world.PendingEventCount() == 0U);
}

TEST_CASE("world rejects out-of-height air writes instead of reporting a no-op success")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 1 });

	CHECK(!world.SetBlock(0, -1, 0, ve::blocks::BlockId::Air));
	CHECK(!world.SetBlock(0, ::Chunk::CHUNK_HEIGHT, 0, ve::blocks::BlockId::Air));
}
