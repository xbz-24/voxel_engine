#include "PublicStaticSceneTestSupport.h"

#include "EngineConfiguration.h"

#include <doctest/doctest.h>

namespace
{
	ve::engine::EngineCreateInfo CreateDirectInfo(const std::filesystem::path& model_path)
	{
		ve::engine::EngineCreateInfo create_info{};
		create_info.render_backend.preferred_api = ve::rendering::GraphicsApi::DirectX12;
		create_info.static_model_scene =
			ve::engine::StaticModelSceneConfiguration{ model_path, true };
		return create_info;
	}
}

TEST_CASE("static model scene accepts the effective OpenGL fallback")
{
	ve::tests::TemporaryStaticSceneObj model{ "backend_opengl_fallback" };
	ve::engine::EngineCreateInfo create_info = CreateDirectInfo(model.Path());
	create_info.render_backend.allow_opengl_compatibility_fallback = true;

	CHECK(ve::engine::ValidateEngineCreateInfo(create_info).empty());
}

TEST_CASE("static model scene rejects BestAvailable when it selects Vulkan")
{
	ve::tests::TemporaryStaticSceneObj model{ "backend_best_available" };
	ve::engine::EngineCreateInfo create_info = CreateDirectInfo(model.Path());
	create_info.render_backend.selection_policy =
		ve::rendering::RenderBackendSelectionPolicy::BestAvailable;

	CHECK(ve::tests::ContainsIssue(ve::engine::ValidateEngineCreateInfo(create_info),
		"static_model_scene requires the selected graphics api to be OpenGLCompatibility"));
}
