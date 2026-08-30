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

