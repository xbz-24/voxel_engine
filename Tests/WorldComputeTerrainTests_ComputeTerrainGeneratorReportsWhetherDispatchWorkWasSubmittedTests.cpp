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
#include "ComputeTerrainTestSupport.h"

#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include <vector>

TEST_CASE("compute terrain generator reports whether dispatch work was submitted")
{
	RecordingComputeDispatcher supportedDispatcher(true);
	ve::world::generation::ComputeTerrainGenerator generator(
		ve::rendering::ComputeProgramHandle{ 7U },
		&supportedDispatcher);

	CHECK(generator.Dispatch(2U, 3U, 4U));
	CHECK(supportedDispatcher.dispatchCount == 1);
	CHECK(supportedDispatcher.lastProgram.value == 7U);
	CHECK(supportedDispatcher.lastDispatchSize.groups_x == 2U);
	CHECK(supportedDispatcher.lastDispatchSize.groups_y == 3U);
	CHECK(supportedDispatcher.lastDispatchSize.groups_z == 4U);
	CHECK(!generator.Dispatch(0U, 1U, 1U));
	CHECK(supportedDispatcher.dispatchCount == 1);
}

