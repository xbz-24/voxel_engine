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
#include "WorldTerrainTestSupport.h"

#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include <vector>

TEST_CASE("desert terrain biome selects a dry default surface palette")
{
	ve::world::TerrainGenerationSettings desertTerrainSettings;
	desertTerrainSettings.baseSurfaceHeight = 8;
	desertTerrainSettings.spawnBiome = ve::world::TerrainSpawnBiome::Desert;

	ve::world::terrain::BlockStorage desertTerrainBlocks{};
	ve::world::terrain::GenerateChunkTerrain(0, 0, desertTerrainSettings, desertTerrainBlocks);

	const int localBlockX = 1;
	const int localBlockZ = 1;
	const int desertSurfaceY = HighestSolidBlockY(desertTerrainBlocks, localBlockX, localBlockZ);

	REQUIRE(desertSurfaceY >= 0);
	CHECK(desertTerrainBlocks[localBlockX][desertSurfaceY][localBlockZ] == ve::blocks::BlockId::Sand);
}
