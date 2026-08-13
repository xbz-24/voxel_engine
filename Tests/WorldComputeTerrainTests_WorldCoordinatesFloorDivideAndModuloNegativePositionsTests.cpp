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

TEST_CASE("world coordinates floor divide and modulo negative positions")
{
	CHECK(ve::world::coordinates::FloorDiv(0, 16) == 0);
	CHECK(ve::world::coordinates::FloorDiv(15, 16) == 0);
	CHECK(ve::world::coordinates::FloorDiv(16, 16) == 1);
	CHECK(ve::world::coordinates::FloorDiv(-1, 16) == -1);
	CHECK(ve::world::coordinates::FloorDiv(-16, 16) == -1);
	CHECK(ve::world::coordinates::FloorDiv(-17, 16) == -2);

	CHECK(ve::world::coordinates::PositiveMod(-1, 16) == 15);
	CHECK(ve::world::coordinates::PositiveMod(-16, 16) == 0);
	CHECK(ve::world::coordinates::PositiveMod(17, 16) == 1);
}

