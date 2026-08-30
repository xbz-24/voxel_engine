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

