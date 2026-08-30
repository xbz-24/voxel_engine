#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public world api normalizes block boxes")
{
	const voxel::BlockBox box = voxel::BlockBox::Between({ 4, 9, 2 }, { -1, 3, 8 });

	CHECK(box.minimum.x == -1);
	CHECK(box.minimum.y == 3);
	CHECK(box.minimum.z == 2);
	CHECK(box.maximum.x == 4);
	CHECK(box.maximum.y == 9);
	CHECK(box.maximum.z == 8);
}

