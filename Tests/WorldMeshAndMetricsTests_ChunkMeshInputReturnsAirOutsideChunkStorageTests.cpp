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

TEST_CASE("chunk mesh input returns air outside chunk storage")
{
	std::vector<ve::blocks::BlockId> blocks(ve::world::mesh::ChunkBlockCount, ve::blocks::BlockId::Air);
	blocks.front() = ve::blocks::BlockId::Dirt;
	blocks.back() = ve::blocks::BlockId::Stone;
	const ve::world::mesh::ChunkMeshInput input{ 0, 0, blocks };

	CHECK(input.GetBlock(0, 0, 0) == ve::blocks::BlockId::Dirt);
	CHECK(input.GetBlock(15, 127, 15) == ve::blocks::BlockId::Stone);
	CHECK(input.GetBlock(-1, 0, 0) == ve::blocks::BlockId::Air);
	CHECK(input.GetBlock(16, 0, 0) == ve::blocks::BlockId::Air);
	CHECK(input.GetBlock(0, 128, 0) == ve::blocks::BlockId::Air);
	CHECK(input.GetBlock(0, 0, 16) == ve::blocks::BlockId::Air);
}

namespace
{
	std::size_t LocalBlockIndex(int local_block_x, int local_block_y, int local_block_z)
	{
		return ve::core::ToIndex(
			(local_block_x * ve::world::terrain::ChunkHeight + local_block_y) *
			ve::world::terrain::ChunkDepth + local_block_z);
	}

	void SetLocalBlock(
		std::vector<ve::blocks::BlockId>& chunk_blocks,
		int local_block_x,
		int local_block_y,
		int local_block_z,
		ve::blocks::BlockId block_id)
	{
		chunk_blocks[LocalBlockIndex(local_block_x, local_block_y, local_block_z)] = block_id;
	}
}

