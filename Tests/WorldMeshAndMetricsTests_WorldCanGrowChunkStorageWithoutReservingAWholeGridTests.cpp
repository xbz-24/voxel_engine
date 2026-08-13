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

