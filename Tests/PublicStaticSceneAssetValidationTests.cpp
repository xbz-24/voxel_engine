#include "PublicStaticSceneTestSupport.h"

#include <doctest/doctest.h>

TEST_CASE("public static OBJ scene rejects unsupported backend and lookup policies")
{
	ve::tests::TemporaryStaticSceneObj model{ "backend" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());
	std::string expected;

	SUBCASE("Vulkan")
	{
		config.UseVulkan();
		expected = "Static model scenes require OpenGLCompatibility";
	}
	SUBCASE("DirectX12")
	{
		config.UseDirectX12();
		expected = "Static model scenes require OpenGLCompatibility";
	}
	SUBCASE("search roots")
	{
		config.assets.SearchRoot(model.Path().parent_path().string());
		expected = "Static model scenes do not support asset search roots";
	}

	CHECK(ve::tests::ContainsIssue(config.Validate(), expected));
}

TEST_CASE("public static OBJ scene rejects unsupported asset kinds and sources")
{
	ve::tests::TemporaryStaticSceneObj model{ "asset_kind" };
	voxel::EngineConfig config = ve::tests::StaticSceneConfig(model.Path());
	std::string expected;

	SUBCASE("texture")
	{
		config.assets.Texture("texture", model.Path().string());
		expected = "Static model scenes do not support texture assets";
	}
	SUBCASE("sound")
	{
		config.assets.Sound("sound", model.Path().string());
		expected = "Static model scenes do not support sound assets";
	}
	SUBCASE("embedded")
	{
		config.assets.models[0].source = voxel::AssetSource::Embedded({ 1U });
		expected = "Static model scenes require a FilePath model asset";
	}
	SUBCASE("archive")
	{
		config.assets.models[0].source = voxel::AssetSource::Archive("models.pack", "triangle.obj");
		expected = "Static model scenes require a FilePath model asset";
	}
	SUBCASE("hot reload")
	{
		config.assets.models[0].source.EnableHotReload();
		expected = "Static model scenes do not support model hot reload";
	}
	SUBCASE("irrelevant FilePath payload")
	{
		config.assets.models[0].source.embedded_data.push_back(1U);
		expected =
			"Static model scene FilePath sources must not contain archive or embedded payloads";
	}

	CHECK(ve::tests::ContainsIssue(config.Validate(), expected));
}
