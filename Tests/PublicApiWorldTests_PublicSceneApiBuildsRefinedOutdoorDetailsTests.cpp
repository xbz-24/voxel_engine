#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public scene api builds refined outdoor details")
{
	const voxel::WorldConfig scene = voxel::Scene()
		.AddPond(0, 63, 0)
		.AddGarden(8, 63, 0)
		.AddBridgeX(-3, 3, 64, 0)
		.AddBridgeZ(0, 64, -3, 3);

	REQUIRE(scene.edits.size() == 13);
	CHECK(scene.edits[0].block == voxel::MossyCobblestone);
	CHECK(scene.edits[1].block == voxel::Water);
	CHECK(scene.edits[2].block == voxel::Air);
	CHECK(scene.edits[3].block == voxel::Dirt);
	CHECK(scene.edits[4].block == voxel::HayBlock);
	CHECK(scene.edits[7].block == voxel::OakPlanks);
	CHECK(scene.edits[8].block == voxel::OakLog);
	CHECK(scene.edits[10].block == voxel::OakPlanks);
}

