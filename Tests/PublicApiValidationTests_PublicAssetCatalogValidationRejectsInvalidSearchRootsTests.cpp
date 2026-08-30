#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public asset catalog validation rejects invalid search roots")
{
	voxel::AssetCatalog assets{};
	assets.SearchRoot("");

	const std::vector<std::string> issues = assets.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"asset search root must not be empty") != issues.end());
}

