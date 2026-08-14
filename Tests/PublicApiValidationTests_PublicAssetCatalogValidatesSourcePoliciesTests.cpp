#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public asset catalog validates source policies")
{
	voxel::AssetSource unknown_storage = voxel::AssetSource::File("texture.png");
	unknown_storage.storage = static_cast<voxel::AssetStorage>(255);
	voxel::AssetCatalog assets{};
	assets.Texture("empty-embedded", voxel::AssetSource::Embedded({}))
		.Model("bad-archive", voxel::AssetSource::Archive("", "crate.obj"))
		.Sound("bad-hot-reload", voxel::AssetSource::Embedded({ 1U }).EnableHotReload())
		.Texture("unknown-storage", unknown_storage);

	const std::vector<std::string> issues = assets.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"texture embedded asset data must not be empty") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"model archive path must not be empty") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"sound asset hot reload requires a filesystem path") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"texture asset storage is not known") != issues.end());
}

