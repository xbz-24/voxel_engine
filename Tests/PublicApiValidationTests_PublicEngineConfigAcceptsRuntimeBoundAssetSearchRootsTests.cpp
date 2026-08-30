#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public engine config accepts runtime-bound asset search roots")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithAssets(voxel::AssetCatalog{}.SearchRoot("assets"));

	CHECK(config.IsValid());
}

