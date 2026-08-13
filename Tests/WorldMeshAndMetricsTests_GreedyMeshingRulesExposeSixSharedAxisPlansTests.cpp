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
#include "WorldMeshTestSupport.h"

#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include <vector>

TEST_CASE("greedy meshing rules expose six shared axis plans")
{
	constexpr std::array axis_plans = ve::world::mesh::GreedyMeshingRules::AxisPlans();

	CHECK(axis_plans.size() == 6U);
	CHECK(axis_plans.front().direction == ve::world::mesh::MeshFaceDirection::Top);
	CHECK(axis_plans.front().block_face == ve::blocks::BlockFace::Top);
	CHECK(axis_plans.back().direction == ve::world::mesh::MeshFaceDirection::Left);
	CHECK(axis_plans.back().normal_axis == 0);
}
