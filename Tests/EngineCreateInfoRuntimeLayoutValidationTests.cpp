#include <doctest/doctest.h>

#include "EngineConfiguration.h"
#include "RuntimeLayoutTestSupport.h"

#include <algorithm>
#include <string>
#include <string_view>

namespace
{
	[[nodiscard]] bool ContainsIssue(const std::vector<std::string>& issues,
		std::string_view expected)
	{
		return std::find(issues.begin(), issues.end(), expected) != issues.end();
	}
}

TEST_CASE("engine create info accepts exact OpenGL assets without Vulkan shaders")
{
	ve::tests::TemporaryRuntimeLayout files{ "internal_opengl" };
	ve::engine::EngineCreateInfo create_info{};
	create_info.render_backend.preferred_api = ve::rendering::GraphicsApi::OpenGLCompatibility;
	create_info.asset_directory = files.Assets();

	CHECK(ve::engine::ValidateEngineCreateInfo(create_info).empty());
}

TEST_CASE("engine create info requires shaders for an explicit Vulkan layout")
{
	ve::tests::TemporaryRuntimeLayout files{ "internal_vulkan" };
	ve::engine::EngineCreateInfo create_info{};
	create_info.asset_directory = files.Assets();
	const std::vector<std::string> missing_shader_issues =
		ve::engine::ValidateEngineCreateInfo(create_info);
	CHECK(ContainsIssue(missing_shader_issues,
		"vulkan_shader_directory is required for an explicit Vulkan runtime layout"));

	files.WriteValidShaderBundle();
	create_info.vulkan_shader_directory = files.Shaders();
	CHECK(ve::engine::ValidateEngineCreateInfo(create_info).empty());
}

TEST_CASE("runtime layout validates the effectively selected backend")
{
	ve::tests::TemporaryRuntimeLayout files{ "internal_selected_backend" };
	ve::engine::EngineCreateInfo create_info{};
	create_info.render_backend.preferred_api = ve::rendering::GraphicsApi::DirectX12;
	create_info.render_backend.selection_policy =
		ve::rendering::RenderBackendSelectionPolicy::BestAvailable;
	create_info.asset_directory = files.Assets();

	CHECK(ContainsIssue(ve::engine::ValidateEngineCreateInfo(create_info),
		"vulkan_shader_directory is required for an explicit Vulkan runtime layout"));
}

TEST_CASE("engine create info rejects invalid explicit runtime paths")
{
	ve::tests::TemporaryRuntimeLayout files{ "internal_invalid" };
	ve::engine::EngineCreateInfo create_info{};
	create_info.asset_directory = std::filesystem::path{};
	create_info.vulkan_shader_directory = files.Shaders();
	const std::vector<std::string> issues = ve::engine::ValidateEngineCreateInfo(create_info);

	CHECK(ContainsIssue(issues, "asset_directory must not be empty when explicitly configured"));
	CHECK(ContainsIssue(issues,
		"vulkan_shader_directory must contain the complete Vulkan shader bundle "
		"with recognizable SPIR-V headers"));
}
