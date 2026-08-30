#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public config validation reports invalid startup values")
{
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow("Invalid", 0, -1)
		.WithWorldSizeChunks(0)
		.WithRenderDistanceChunks(-1)
		.UseDirectX12()
		.WithAssets(voxel::AssetCatalog{}
			.Texture("duplicate", "a.png")
			.Texture("duplicate", "b.png")
			.Model("", ""));

	const std::vector<std::string> issues = invalid.Validate();

	CHECK(!invalid.IsValid());
	CHECK(issues.size() == 9);
	CHECK(std::find(issues.begin(), issues.end(),
		"AssetCatalog runtime loading is not implemented") != issues.end());
}

