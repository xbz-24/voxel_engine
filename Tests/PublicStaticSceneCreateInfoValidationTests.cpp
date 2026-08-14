#include "PublicStaticSceneTestSupport.h"

#include "EngineConfiguration.h"

#include <doctest/doctest.h>

namespace
{
	[[nodiscard]] ve::engine::EngineCreateInfo CreateInfoFor(
		const std::filesystem::path& model_path)
	{
		ve::engine::EngineCreateInfo create_info{};
		create_info.render_backend.preferred_api =
			ve::rendering::GraphicsApi::OpenGLCompatibility;
		create_info.static_model_scene =
			ve::engine::StaticModelSceneConfiguration{ model_path, true };
		return create_info;
	}
}

TEST_CASE("engine create info accepts an exact OpenGL static OBJ scene")
{
	ve::tests::TemporaryStaticSceneObj model{ "internal_accepted" };
	const ve::engine::EngineCreateInfo create_info = CreateInfoFor(model.Path());

	CHECK(ve::engine::ValidateEngineCreateInfo(create_info).empty());
}

TEST_CASE("engine create info rejects a static OBJ scene for Vulkan")
{
	ve::tests::TemporaryStaticSceneObj model{ "internal_vulkan" };
	ve::engine::EngineCreateInfo create_info = CreateInfoFor(model.Path());
	create_info.render_backend.preferred_api = ve::rendering::GraphicsApi::Vulkan;

	CHECK(ve::tests::ContainsIssue(ve::engine::ValidateEngineCreateInfo(create_info),
		"static_model_scene requires the selected graphics api to be OpenGLCompatibility"));
}

TEST_CASE("engine create info rejects search roots for an exact static scene")
{
	ve::tests::TemporaryStaticSceneObj model{ "internal_search_roots" };
	ve::engine::EngineCreateInfo create_info = CreateInfoFor(model.Path());
	create_info.asset_search_roots.push_back(model.Path().parent_path());

	CHECK(ve::tests::ContainsIssue(ve::engine::ValidateEngineCreateInfo(create_info),
		"static_model_scene does not support asset_search_roots"));
}

TEST_CASE("engine create info rejects invalid static scene paths")
{
	ve::tests::TemporaryStaticSceneObj model{ "internal_path" };
	ve::engine::EngineCreateInfo create_info = CreateInfoFor(model.Path());
	std::string expected;

	SUBCASE("empty")
	{
		create_info.static_model_scene->model_path.clear();
		expected = "static_model_scene.model_path must not be empty";
	}
	SUBCASE("relative")
	{
		create_info.static_model_scene->model_path = "triangle.obj";
		expected = "static_model_scene.model_path must be absolute";
	}
	SUBCASE("wrong extension")
	{
		create_info.static_model_scene->model_path.replace_extension(".gltf");
		expected = "static_model_scene.model_path must have a .obj extension";
	}
	SUBCASE("missing")
	{
		create_info.static_model_scene->model_path =
			model.Path().parent_path() / "missing_internal_scene.obj";
		expected = "static_model_scene.model_path must name an existing regular file";
	}

	CHECK(ve::tests::ContainsIssue(ve::engine::ValidateEngineCreateInfo(create_info), expected));
}
