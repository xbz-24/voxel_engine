#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public scene api exposes semantic objects")
{
	const voxel::WorldConfig scene = voxel::Scene()
		.AddHouse(0, 64, 0)
		.AddLightPost(6, 64, -4)
		.AddPathZ(0, 63, -12, -4)
		.ClearBox(-1, 65, -4, 1, 66, -4);

	REQUIRE(scene.edits.size() == 13);
	CHECK(scene.edits[0].block == voxel::OakPlanks);
	CHECK(scene.edits[5].block == voxel::Bricks);
	CHECK(scene.edits[6].block == voxel::Air);
	CHECK(scene.edits[9].block == voxel::OakLog);
	CHECK(scene.edits[10].block == voxel::SeaLantern);
	CHECK(scene.edits[11].block == voxel::Gravel);
}

