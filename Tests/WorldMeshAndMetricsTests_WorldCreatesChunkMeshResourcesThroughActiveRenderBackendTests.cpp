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

TEST_CASE("world creates chunk mesh resources through active render backend")
{
	CountingRenderBackend render_backend;
	ve::world::World world(ve::world::CreateInfoForSquareWorld(2));
	world.SetRenderBackend(&render_backend);

	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 2 });
	CHECK(render_backend.created_mesh_resource_count == 4);

	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ 1 });
	CHECK(render_backend.created_mesh_resource_count == 5);
}
