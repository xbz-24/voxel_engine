#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public world api supports typed coordinate units")
{
	const voxel::BlockPosition position = voxel::At(
		voxel::BlockCoord(1),
		voxel::BlockCoord(64),
		voxel::BlockCoord(-2));
	const voxel::ChunkCoordinate chunk_coordinate = voxel::ChunkCoord(3);
	const voxel::WorldConfig world = voxel::World(voxel::Chunks(5))
		.WithSizeChunks(voxel::Chunks(6))
		.SetBlock(position, voxel::DiamondOre);

	CHECK(position.x == 1);
	CHECK(position.y == 64);
	CHECK(position.z == -2);
	CHECK(voxel::ChunkCoordinateValue(chunk_coordinate) == 3);
	CHECK(world.size_chunks == 6);
	REQUIRE(world.edits.size() == 1U);
	CHECK(world.edits.front().position.x == 1);
	CHECK(world.edits.front().block == voxel::DiamondOre);
}

