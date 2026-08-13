#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public engine config validation reports missing asset and material references")
{
	voxel::AssetCatalog assets{};
	assets.Texture("albedo", "albedo.png");

	voxel::MaterialLibrary materials{};
	materials.Add(voxel::Material::Named("painted")
		.Texture("albedo")
		.NormalTexture("missing-normal"));

	voxel::SceneGraph scene_graph{};
	(void)scene_graph.AddEntity(voxel::Entity::Named("crate")
		.Model("missing-crate")
		.Material("missing-material"));

	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithAssets(assets)
		.WithMaterials(materials)
		.WithSceneGraph(scene_graph);

	const std::vector<std::string> issues = config.Validate();

	CHECK(std::find(issues.begin(), issues.end(),
		"material 'painted' references missing texture asset: missing-normal") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'crate' references missing model asset: missing-crate") != issues.end());
	CHECK(std::find(issues.begin(), issues.end(),
		"scene entity 'crate' references missing material: missing-material") != issues.end());
}

