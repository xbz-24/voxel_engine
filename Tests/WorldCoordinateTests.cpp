#include <doctest/doctest.h>

#include "ChunkMeshInput.h"
#include "WorldCoordinates.h"

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
