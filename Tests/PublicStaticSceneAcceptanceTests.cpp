#include "PublicStaticSceneTestSupport.h"

#include <doctest/doctest.h>

TEST_CASE("public OpenGL static OBJ scene accepts the exact supported slice")
{
	ve::tests::TemporaryStaticSceneObj model{ "accepted" };
	const voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());

	CHECK(config.Validate().empty());
	CHECK(config.IsValid());
}

TEST_CASE("public static scene support does not claim broad runtime capabilities")
{
	const voxel::SdkFeatures features = voxel::Features();

	CHECK_FALSE(features.asset_catalog_runtime_loading);
	CHECK_FALSE(features.material_library_runtime_binding);
	CHECK_FALSE(features.scene_graph_runtime_rendering);
}

TEST_CASE("asset search roots remain valid when no static scene is requested")
{
	voxel::AssetCatalog assets{};
	assets.SearchRoot("assets");
	const voxel::EngineConfig config = voxel::EngineConfig::Default().WithAssets(std::move(assets));

	CHECK(config.Validate().empty());
}

TEST_CASE("custom environment without entities remains capability gated")
{
	voxel::SceneGraph scene{};
	scene.EnvironmentSettings(voxel::Environment{}.TimeOfDay(18.0f));
	const voxel::EngineConfig config = voxel::EngineConfig::Default().WithSceneGraph(std::move(scene));

	CHECK(ve::tests::ContainsIssue(config.Validate(),
		"SceneGraph runtime rendering is not implemented"));
}
