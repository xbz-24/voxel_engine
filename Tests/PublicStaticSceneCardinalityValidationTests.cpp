#include "PublicStaticSceneTestSupport.h"

#include <doctest/doctest.h>

TEST_CASE("public static OBJ scene requires exactly one model")
{
	ve::tests::TemporaryStaticSceneObj model{ "model_count" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());

	SUBCASE("second model")
	{
		config.assets.Model("second", model.Path().string());
	}

	CHECK(ve::tests::ContainsIssue(config.Validate(),
		"Static model scenes require exactly one model asset"));
}

TEST_CASE("public scene entity without a model remains broadly capability gated")
{
	ve::tests::TemporaryStaticSceneObj model{ "missing_model" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());
	config.assets.models.clear();

	CHECK(ve::tests::ContainsIssue(config.Validate(),
		"SceneGraph runtime rendering is not implemented"));
}

TEST_CASE("public static OBJ scene rejects ambiguous FilePath payloads")
{
	ve::tests::TemporaryStaticSceneObj model{ "ambiguous_path" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());
	config.assets.models[0].path = (model.Path().parent_path() / "other.obj").string();

	CHECK(ve::tests::ContainsIssue(config.Validate(),
		"Static model scene model path and source location must match"));
}
