#include <doctest/doctest.h>

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "ChunkGpuMesh.h"
#include "ChunkMeshBuilder.h"
#include "ChunkMeshInput.h"
#include "ComputeTerrainGenerator.h"
#include "GreedyChunkMesher.h"
#include "RenderBackend.h"
#include "World.h"
#include "WorldBlockEdit.h"
#include "WorldCoordinates.h"
#include "TestTypeHelpers.h"

#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include <vector>

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

