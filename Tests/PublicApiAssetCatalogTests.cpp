#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <string>
#include <vector>

TEST_CASE("public asset catalog configures supported sources")
{
	voxel::AssetCatalog assets{};
	assets.SearchRoot("assets")
		.Texture("grass", voxel::AssetSource::File("assets/grass.png").EnableHotReload())
		.Texture("normal", "assets/normal.png")
		.Texture("roughness", "assets/roughness.png")
		.Texture("metallic", "assets/metallic.png")
		.Texture("occlusion", "assets/occlusion.png")
		.Texture("emissive", "assets/emissive.png")
		.Model("crate", voxel::AssetSource::Archive("assets/models.pack", "crate.obj"))
		.Sound("click", voxel::AssetSource::Embedded({ 1U, 2U, 3U }));

	const voxel::EngineConfig config = voxel::EngineConfig::Default().WithAssets(assets);

	CHECK(config.assets.textures.size() == 6);
	REQUIRE(config.assets.search_roots.size() == 1);
	CHECK(config.assets.search_roots[0] == "assets");
	CHECK(config.assets.models.size() == 1);
	CHECK(config.assets.sounds.size() == 1);
	CHECK(config.assets.textures[0].source.hot_reload);
	CHECK(config.assets.models[0].source.storage == voxel::AssetStorage::PackagedArchive);
	CHECK(config.assets.sounds[0].source.storage == voxel::AssetStorage::EmbeddedData);

	const std::vector<std::string> issues = config.Validate();
	CHECK(std::find(issues.begin(), issues.end(),
		"AssetCatalog runtime loading is not implemented") != issues.end());
}
