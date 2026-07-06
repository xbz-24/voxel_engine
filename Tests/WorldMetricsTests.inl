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

TEST_CASE("world tracks dirty chunk metadata outside chunk storage")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(2));
	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 2 });

	CHECK(world.DirtyChunks().empty());
	CHECK(world.SetBlock(15, 1, 15, ve::blocks::BlockId::Air));

	const std::span<const ve::world::DirtyChunkMetadata> dirty_chunks = world.DirtyChunks();
	REQUIRE(dirty_chunks.size() == 3U);
	CHECK(dirty_chunks[0].chunk_x == 0);
	CHECK(dirty_chunks[0].chunk_z == 0);
	CHECK(dirty_chunks[0].has_authored_edits);
	CHECK(dirty_chunks[1].chunk_x == 1);
	CHECK(dirty_chunks[2].chunk_z == 1);

	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	CHECK(world.DirtyChunks().empty());
}

TEST_CASE("world event contract includes lifecycle tooling events")
{
	const ve::world::WorldEvent chunk_loaded{ ve::world::ChunkLoadedEvent{ 2, 3 } };
	const ve::world::WorldEvent chunk_unloaded{ ve::world::ChunkUnloadedEvent{ 4, 5 } };
	const ve::world::WorldEvent entity_changed{ ve::world::EntityChangedEvent{ 42U, "transform" } };
	const ve::world::WorldEvent region_saved{ ve::world::RegionSavedEvent{ -1, -2, 3, 4, "region-0.voxels" } };

	CHECK(chunk_loaded.Type() == ve::world::WorldEventType::ChunkLoaded);
	REQUIRE(chunk_loaded.AsChunkLoaded() != nullptr);
	CHECK(chunk_loaded.AsChunkLoaded()->chunkCoordinateX == 2);
	CHECK(chunk_unloaded.Type() == ve::world::WorldEventType::ChunkUnloaded);
	REQUIRE(chunk_unloaded.AsChunkUnloaded() != nullptr);
	CHECK(chunk_unloaded.AsChunkUnloaded()->chunkCoordinateZ == 5);
	CHECK(entity_changed.Type() == ve::world::WorldEventType::EntityChanged);
	REQUIRE(entity_changed.AsEntityChanged() != nullptr);
	CHECK(entity_changed.AsEntityChanged()->entityId == 42U);
	CHECK(entity_changed.AsEntityChanged()->reason == "transform");
	CHECK(region_saved.Type() == ve::world::WorldEventType::RegionSaved);
	REQUIRE(region_saved.AsRegionSaved() != nullptr);
	CHECK(region_saved.AsRegionSaved()->maximumChunkCoordinateZ == 4);
	CHECK(region_saved.AsRegionSaved()->destination == "region-0.voxels");

	const ve::world::WorldEventFilter lifecycle_filter = ve::world::WorldEventFilter::ChunkLifecycleOnly();
	CHECK(lifecycle_filter.Includes(ve::world::WorldEventType::ChunkLoaded));
	CHECK(lifecycle_filter.Includes(ve::world::WorldEventType::ChunkUnloaded));
	CHECK(!lifecycle_filter.Includes(ve::world::WorldEventType::BlockChanged));
	CHECK(ve::world::WorldEventFilter::EntityChangesOnly().Includes(ve::world::WorldEventType::EntityChanged));
	CHECK(ve::world::WorldEventFilter::RegionPersistenceOnly().Includes(ve::world::WorldEventType::RegionSaved));
}

TEST_CASE("world rejects out-of-height air writes instead of reporting a no-op success")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 1 });

	CHECK(!world.SetBlock(0, -1, 0, ve::blocks::BlockId::Air));
	CHECK(!world.SetBlock(0, ::Chunk::CHUNK_HEIGHT, 0, ve::blocks::BlockId::Air));
}

TEST_CASE("chunk tracks procedural and authored content provenance")
{
	Chunk empty_chunk(0, 0, ChunkGenerationMode::Empty);

	CHECK(empty_chunk.Provenance() == ChunkContentProvenance::Empty);
	CHECK(!empty_chunk.HasProceduralTerrain());
	CHECK(!empty_chunk.HasAuthoredEdits());

	empty_chunk.Generate();
	CHECK(empty_chunk.Provenance() == ChunkContentProvenance::ProceduralTerrain);
	CHECK(empty_chunk.HasProceduralTerrain());
	CHECK(!empty_chunk.HasAuthoredEdits());

	CHECK(empty_chunk.SetBlock(0, 1, 0, ve::blocks::BlockId::DiamondOre));
	CHECK(empty_chunk.Provenance() == ChunkContentProvenance::ProceduralTerrainWithAuthoredEdits);
	CHECK(empty_chunk.HasAuthoredEdits());
}
