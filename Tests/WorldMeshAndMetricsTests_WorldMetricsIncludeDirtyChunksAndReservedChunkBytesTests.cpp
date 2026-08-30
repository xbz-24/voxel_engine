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

