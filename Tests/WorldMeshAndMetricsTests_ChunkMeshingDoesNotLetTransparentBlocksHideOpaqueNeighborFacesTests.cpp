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

TEST_CASE("chunk meshing does not let transparent blocks hide opaque neighbor faces")
{
	std::vector<ve::blocks::BlockId> blocks(ve::world::mesh::ChunkBlockCount, ve::blocks::BlockId::Air);
	SetLocalBlock(blocks, 0, 0, 0, ve::blocks::BlockId::Dirt);
	SetLocalBlock(blocks, 1, 0, 0, ve::blocks::BlockId::OakLeaves);

	const ve::blocks::BlockRegistry block_registry(
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);
	const ve::world::mesh::ChunkMeshInput input{ 0, 0, blocks };
	const ve::world::mesh::ChunkMeshBuildResult mesh = ve::world::mesh::BuildChunkMesh(
		input,
		block_registry,
		ve::world::mesh::NeighborMeshInputs{});

	CHECK(mesh.diagnostics.candidate_block_face_count == 12U);
	CHECK(mesh.diagnostics.visible_block_face_count == 11U);
	CHECK(mesh.diagnostics.culled_block_face_count == 1U);
}

