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

TEST_CASE("terrain generation settings control base height and block palette")
{
	ve::world::TerrainGenerationSettings lowTerrainSettings;
	lowTerrainSettings.baseSurfaceHeight = 8;
	lowTerrainSettings.blockPalette.surfaceBlock = ve::blocks::BlockId::Sandstone;
	lowTerrainSettings.blockPalette.subsurfaceBlock = ve::blocks::BlockId::RedSandstone;
	lowTerrainSettings.blockPalette.bedrockBlock = ve::blocks::BlockId::Obsidian;

	ve::world::TerrainGenerationSettings highTerrainSettings = lowTerrainSettings;
	highTerrainSettings.baseSurfaceHeight = 24;

	ve::world::terrain::BlockStorage lowTerrainBlocks{};
	ve::world::terrain::BlockStorage highTerrainBlocks{};
	ve::world::terrain::GenerateChunkTerrain(0, 0, lowTerrainSettings, lowTerrainBlocks);
	ve::world::terrain::GenerateChunkTerrain(0, 0, highTerrainSettings, highTerrainBlocks);

	const int localBlockX = 1;
	const int localBlockZ = 1;
	const int lowTerrainSurfaceY = HighestSolidBlockY(lowTerrainBlocks, localBlockX, localBlockZ);
	const int highTerrainSurfaceY = HighestSolidBlockY(highTerrainBlocks, localBlockX, localBlockZ);

	CHECK(highTerrainSurfaceY > lowTerrainSurfaceY);
	REQUIRE(lowTerrainSurfaceY > 0);
	CHECK(lowTerrainBlocks[localBlockX][lowTerrainSurfaceY][localBlockZ] == ve::blocks::BlockId::Sandstone);
	CHECK(lowTerrainBlocks[localBlockX][lowTerrainSurfaceY - 1][localBlockZ] == ve::blocks::BlockId::RedSandstone);
	CHECK(lowTerrainBlocks[localBlockX][0][localBlockZ] == ve::blocks::BlockId::Obsidian);
}

