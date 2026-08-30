#include "PublicStaticSceneTestSupport.h"

#include <doctest/doctest.h>

#include <limits>

TEST_CASE("public static OBJ scene accepts a finite root translation")
{
	ve::tests::TemporaryStaticSceneObj model{ "translated" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());
	config.scene_graph.entities.front().At({ 3.0f, -2.0f, 5.0f });

	CHECK(config.Validate().empty());
	CHECK(config.IsValid());
}

TEST_CASE("public static OBJ scene rejects a non-finite root translation")
{
	ve::tests::TemporaryStaticSceneObj model{ "non_finite_translation" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());

	SUBCASE("not a number")
	{
		config.scene_graph.entities.front().transform.position.x =
			std::numeric_limits<float>::quiet_NaN();
	}
	SUBCASE("infinity")
	{
		config.scene_graph.entities.front().transform.position.z =
			std::numeric_limits<float>::infinity();
	}

	CHECK(ve::tests::ContainsIssue(config.Validate(),
		"Static model scene entity position must be finite"));
}
