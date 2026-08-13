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

TEST_CASE("compute terrain generator rejects missing backend or program")
{
	RecordingComputeDispatcher unsupportedDispatcher(false);
	ve::world::generation::ComputeTerrainGenerator missingProgram(
		ve::rendering::ComputeProgramHandle{},
		&unsupportedDispatcher);
	ve::world::generation::ComputeTerrainGenerator missingDispatcher(
		ve::rendering::ComputeProgramHandle{ 3U },
		nullptr);

	CHECK(!missingProgram.Dispatch(1U, 1U, 1U));
	CHECK(!missingDispatcher.Dispatch(1U, 1U, 1U));
	CHECK(unsupportedDispatcher.dispatchCount == 0);
}
