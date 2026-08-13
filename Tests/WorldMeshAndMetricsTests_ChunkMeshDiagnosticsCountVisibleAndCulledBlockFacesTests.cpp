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

TEST_CASE("chunk mesh diagnostics count visible and culled block faces")
{
	std::vector<ve::blocks::BlockId> blocks(ve::world::mesh::ChunkBlockCount, ve::blocks::BlockId::Air);
	SetLocalBlock(blocks, 0, 0, 0, ve::blocks::BlockId::Dirt);
	SetLocalBlock(blocks, 1, 0, 0, ve::blocks::BlockId::Dirt);

	const ve::assets::AssetPaths asset_paths{};
	const ve::blocks::BlockRegistry block_registry(
		asset_paths,
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);
	const ve::world::mesh::ChunkMeshInput input{ 0, 0, blocks };
	const ve::world::mesh::NeighborMeshInputs neighbors{};

	const ve::world::mesh::ChunkMeshBuildResult mesh = ve::world::mesh::BuildChunkMesh(input, block_registry, neighbors);

	CHECK(mesh.diagnostics.candidate_block_face_count == 12U);
	CHECK(mesh.diagnostics.visible_block_face_count == 10U);
	CHECK(mesh.diagnostics.culled_block_face_count == 2U);
	CHECK(mesh.diagnostics.merged_face_count == 6U);
	CHECK(mesh.diagnostics.vertex_count == mesh.vertices.size());
	CHECK(mesh.diagnostics.batch_count == mesh.batches.size());
	CHECK(mesh.diagnostics.MergeRatio() == doctest::Approx(10.0f / 6.0f));
}

