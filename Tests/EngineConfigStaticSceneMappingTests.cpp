#include "PublicStaticSceneTestSupport.h"

#include "EngineRuntimeBridge.h"

#include <doctest/doctest.h>

TEST_CASE("static OBJ scene translation owns path visibility and root translation")
{
	ve::tests::TemporaryStaticSceneObj model{ "mapping" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path(), false);
	config.scene_graph.entities.front().At({ 3.0f, -2.0f, 5.0f });
	const ve::engine::EngineCreateInfo translated =
		voxel::detail::DefaultEngineConfigTranslator().Translate(config);

	REQUIRE(translated.static_model_scene.has_value());
	CHECK(translated.static_model_scene->model_path == model.Path());
	CHECK_FALSE(translated.static_model_scene->visible);
	CHECK(translated.static_model_scene->root_translation.x == 3.0f);
	CHECK(translated.static_model_scene->root_translation.y == -2.0f);
	CHECK(translated.static_model_scene->root_translation.z == 5.0f);

	config.assets.models[0].path = "changed.obj";
	config.assets.models[0].source.location = "changed.obj";
	config.scene_graph.entities[0].visible = true;
	config.scene_graph.entities[0].At({ 9.0f, 9.0f, 9.0f });
	CHECK(translated.static_model_scene->model_path == model.Path());
	CHECK_FALSE(translated.static_model_scene->visible);
	CHECK(translated.static_model_scene->root_translation.x == 3.0f);
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
