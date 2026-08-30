#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public scene runner is linkable")
{
	using SceneRunner = int (*)(voxel::WorldConfig);

	SceneRunner runner = &voxel::RunScene;

	CHECK(runner != nullptr);
}
