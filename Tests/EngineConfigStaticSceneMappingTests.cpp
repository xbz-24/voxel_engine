#include "PublicStaticSceneTestSupport.h"

#include "EngineRuntimeBridge.h"

#include <doctest/doctest.h>

TEST_CASE("static OBJ scene translation owns the exact selected path and visibility")
{
	ve::tests::TemporaryStaticSceneObj model{ "mapping" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path(), false);
	const ve::engine::EngineCreateInfo translated =
		voxel::detail::DefaultEngineConfigTranslator().Translate(config);

	REQUIRE(translated.static_model_scene.has_value());
	CHECK(translated.static_model_scene->model_path == model.Path());
	CHECK_FALSE(translated.static_model_scene->visible);

	config.assets.models[0].path = "changed.obj";
	config.assets.models[0].source.location = "changed.obj";
	config.scene_graph.entities[0].visible = true;
	CHECK(translated.static_model_scene->model_path == model.Path());
	CHECK_FALSE(translated.static_model_scene->visible);
}

TEST_CASE("static OBJ scene translation remains empty without an exact supported selection")
{
	ve::tests::TemporaryStaticSceneObj model{ "mapping_gate" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());
	config.UseVulkan();

	const ve::engine::EngineCreateInfo translated =
		voxel::detail::DefaultEngineConfigTranslator().Translate(config);
	CHECK_FALSE(translated.static_model_scene.has_value());
}

TEST_CASE("default public config does not create a private static scene")
{
	const ve::engine::EngineCreateInfo translated =
		voxel::detail::DefaultEngineConfigTranslator().Translate(voxel::EngineConfig::Default());

	CHECK_FALSE(translated.static_model_scene.has_value());
}

TEST_CASE("static OBJ translation preserves intent for private filesystem revalidation")
{
	ve::tests::TemporaryStaticSceneObj model{ "mapping_revalidation" };
	const voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());
	std::filesystem::remove(model.Path());

	const ve::engine::EngineCreateInfo translated =
		voxel::detail::DefaultEngineConfigTranslator().Translate(config);
	REQUIRE(translated.static_model_scene.has_value());
	CHECK(ve::tests::ContainsIssue(ve::engine::ValidateEngineCreateInfo(translated),
		"static_model_scene.model_path must name an existing regular file"));
}
