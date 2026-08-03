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
	CHECK(metrics.chunkStoragePolicy == ve::world::ChunkStoragePolicy::FixedReserve);
}

TEST_CASE("world can grow chunk storage without reserving a whole grid")
{
	ve::world::WorldCreateInfo create_info = ve::world::CreateInfoForStreamingWorld();
	create_info.chunkCapacity = 64U;
	ve::world::World world(create_info);

	CHECK(world.Metrics().reservedChunkCapacity == 0U);
	CHECK(world.Metrics().levelArenaCapacityBytes == 0U);

	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 2 });
	const ve::world::WorldMetrics metrics = world.Metrics();

	CHECK(metrics.loadedChunkCount == 4U);
	CHECK(metrics.reservedChunkCapacity < create_info.chunkCapacity);
	CHECK(metrics.levelArenaUsedBytes == 0U);
	CHECK(metrics.chunkStoragePolicy == ve::world::ChunkStoragePolicy::GrowOnDemand);
}

TEST_CASE("world respawn clears stale pending events")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(2));

	const std::uint64_t initial_storage_revision = world.ChunkStorageRevision();
	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 2 });
	const std::uint64_t first_spawn_storage_revision = world.ChunkStorageRevision();
	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	const std::uint64_t second_spawn_storage_revision = world.ChunkStorageRevision();
	std::vector<ve::world::WorldEvent> generated_events_after_respawn = world.DrainEvents();

	CHECK(first_spawn_storage_revision > initial_storage_revision);
	CHECK(second_spawn_storage_revision > first_spawn_storage_revision);
	REQUIRE(world.Chunks().size() == 1U);
	CHECK(world.Chunks().front().GetChunkX() == 0);
	CHECK(world.Chunks().front().GetChunkZ() == 0);
	CHECK(generated_events_after_respawn.size() == 1U);
	REQUIRE(!generated_events_after_respawn.empty());
	CHECK(generated_events_after_respawn.front().Type() == ve::world::WorldEventType::ChunkGenerated);
	const ve::world::ChunkGeneratedEvent* generated_event = generated_events_after_respawn.front().AsChunkGenerated();
	REQUIRE(generated_event != nullptr);
	CHECK(generated_event->chunkCoordinateX == 0);
	CHECK(generated_event->chunkCoordinateZ == 0);

	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	CHECK(world.PendingEventCount() == 0U);
	CHECK(world.ChunkStorageRevision() > second_spawn_storage_revision);
}

TEST_CASE("world event drain can filter event types")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));

	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	CHECK(world.SetBlock(0, 1, 0, ve::blocks::BlockId::Air));

	std::vector<ve::world::WorldEvent> block_events =
		world.DrainEvents(ve::world::WorldEventFilter::BlockChangesOnly());
	REQUIRE(block_events.size() == 1U);
	CHECK(block_events.front().Type() == ve::world::WorldEventType::BlockChanged);
	CHECK(world.PendingEventCount() == 1U);

	std::vector<ve::world::WorldEvent> chunk_events = world.DrainEvents();
	REQUIRE(chunk_events.size() == 1U);
	CHECK(chunk_events.front().Type() == ve::world::WorldEventType::ChunkGenerated);
	CHECK(world.PendingEventCount() == 0U);
}

