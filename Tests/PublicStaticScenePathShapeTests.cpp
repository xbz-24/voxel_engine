#include "PublicStaticSceneTestSupport.h"

#include <doctest/doctest.h>

TEST_CASE("public static OBJ scene requires an absolute existing regular file")
{
	ve::tests::TemporaryStaticSceneObj model{ "path_shape" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());
	std::string expected;

	SUBCASE("relative")
	{
		config.assets.models[0].path = "triangle.obj";
		config.assets.models[0].source.location = "triangle.obj";
		expected = "Static model scene model path must be absolute";
	}
	SUBCASE("missing")
	{
		const auto missing = model.Path().parent_path() / "missing_static_scene.obj";
		config.assets.models[0].path = missing.string();
		config.assets.models[0].source.location = missing.string();
		expected = "Static model scene model path must name an existing regular file";
	}
	SUBCASE("directory")
	{
		std::filesystem::remove(model.Path());
		std::filesystem::create_directory(model.Path());
		expected = "Static model scene model path must name an existing regular file";
	}

	CHECK(ve::tests::ContainsIssue(config.Validate(), expected));
}

TEST_CASE("public static OBJ scene rejects another regular model extension")
{
	ve::tests::TemporaryStaticSceneObj model("wrong_extension", ".gltf");
	const voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());

	CHECK(ve::tests::ContainsIssue(config.Validate(),
		"Static model scene model path must have a .obj extension"));
}
