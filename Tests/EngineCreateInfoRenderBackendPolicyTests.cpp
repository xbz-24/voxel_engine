#include <doctest/doctest.h>

#include "EngineConfiguration.h"
#include "RuntimeLayoutTestSupport.h"

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
	[[nodiscard]] bool HasIssue(const std::vector<std::string>& issues,
		std::string_view expected)
	{
		return std::find(issues.begin(), issues.end(), expected) != issues.end();
	}
}

TEST_CASE("engine create info rejects headless until the runtime is window independent")
{
	ve::engine::EngineCreateInfo create_info{};
	create_info.render_backend.selection_policy =
		ve::rendering::RenderBackendSelectionPolicy::Headless;
	const std::vector<std::string> issues = ve::engine::ValidateEngineCreateInfo(create_info);

	CHECK(HasIssue(issues,
		"render_backend.selection_policy Headless is not implemented by the windowed runtime"));
	CHECK_FALSE(ve::engine::MakeValidatedEngineCreateInfo(std::move(create_info)).has_value());
}

TEST_CASE("engine create info validates resources against the effective OpenGL fallback")
{
	ve::tests::TemporaryRuntimeLayout files{ "internal_opengl_fallback" };
	ve::engine::EngineCreateInfo create_info{};
	create_info.render_backend.preferred_api = ve::rendering::GraphicsApi::DirectX12;
	create_info.render_backend.allow_opengl_compatibility_fallback = true;
	create_info.asset_directory = files.Assets();

	CHECK(ve::engine::ValidateEngineCreateInfo(create_info).empty());
}

TEST_CASE("engine create info rejects an empty enabled log destination")
{
	ve::engine::EngineCreateInfo create_info{};
	create_info.logging.file_output_enabled = true;

	CHECK(HasIssue(ve::engine::ValidateEngineCreateInfo(create_info),
		"logging.file_output_path must not be empty when file output is enabled"));
}
