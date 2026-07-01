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
	std::vector<ve::world::WorldEvent> generated_events_after_respawn = world.DrainEvents();

	CHECK(generated_events_after_respawn.size() == 1U);
	REQUIRE(!generated_events_after_respawn.empty());
	CHECK(generated_events_after_respawn.front().Type() == ve::world::WorldEventType::ChunkGenerated);
	const ve::world::ChunkGeneratedEvent* generated_event = generated_events_after_respawn.front().AsChunkGenerated();
	REQUIRE(generated_event != nullptr);
	CHECK(generated_event->chunkCoordinateX == 0);
	CHECK(generated_event->chunkCoordinateZ == 0);

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
