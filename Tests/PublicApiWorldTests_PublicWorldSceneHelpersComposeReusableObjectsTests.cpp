#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public world scene helpers compose reusable objects")
{
	const voxel::WorldConfig world = voxel::World()
		.AddTree(0, 64, 0)
		.AddTree(8, 64, 8, voxel::BirchLog, voxel::BirchLeaves);

	REQUIRE(world.edits.size() == 6);
	CHECK(world.edits[0].block == voxel::OakLog);
	CHECK(world.edits[1].block == voxel::OakLeaves);
	CHECK(world.edits[3].block == voxel::BirchLog);
	CHECK(world.edits[4].block == voxel::BirchLeaves);
}

