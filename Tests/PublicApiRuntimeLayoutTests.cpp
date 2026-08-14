#include <doctest/doctest.h>

#include <voxel/EngineConfig.h>

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

TEST_CASE("public runtime layout is copied into startup configuration")
{
	ve::tests::TemporaryRuntimeLayout files{ "public_layout" };
	files.WriteValidShaderBundle();
	voxel::EngineConfig config = voxel::EngineConfig::Default().WithRuntimeLayout(
		voxel::RuntimeLayout{}.AssetsAt(files.Assets().string())
			.VulkanShadersAt(files.Shaders().string()));

	const voxel::EngineStartupConfig startup = config.StartupConfig();
	REQUIRE(startup.runtime_layout.has_value());
	CHECK(startup.runtime_layout->asset_directory == files.Assets().string());
	CHECK(startup.runtime_layout->vulkan_shader_directory == files.Shaders().string());
	CHECK(config.Validate().empty());

	config.UseDiscoveredRuntimeLayout();
	CHECK(!config.runtime_layout.has_value());
}

TEST_CASE("public runtime layout applies backend-specific shader requirements")
{
	ve::tests::TemporaryRuntimeLayout files{ "public_backend" };
	const voxel::RuntimeLayout assets_only = voxel::RuntimeLayout{}.AssetsAt(files.Assets().string());
	const voxel::EngineConfig vulkan = voxel::EngineConfig::Default().WithRuntimeLayout(assets_only);
	const voxel::EngineConfig opengl = voxel::EngineConfig::Default()
		.UseOpenGLCompatibility().WithRuntimeLayout(assets_only);

	CHECK(ContainsIssue(vulkan.Validate(),
		"runtime_layout.vulkan_shader_directory is required for Vulkan"));
	CHECK(opengl.Validate().empty());
}

TEST_CASE("public runtime layout rejects invalid exact directories and shader files")
{
	ve::tests::TemporaryRuntimeLayout files{ "public_invalid" };
	files.WriteValidShaderBundle();
	files.WriteShader("voxel_shadow.vert.spv", 3);
	const voxel::EngineConfig invalid_shader = voxel::EngineConfig::Default()
		.UseOpenGLCompatibility().WithRuntimeLayout(voxel::RuntimeLayout{}
			.AssetsAt(files.Assets().string()).VulkanShadersAt(files.Shaders().string()));
	const voxel::EngineConfig missing_assets = voxel::EngineConfig::Default()
		.UseOpenGLCompatibility().WithRuntimeLayout(voxel::RuntimeLayout{}
			.AssetsAt((files.Assets() / "missing").string()));

	CHECK(ContainsIssue(invalid_shader.Validate(),
		"voxel_shadow.vert.spv must contain a SPIR-V header and whole 32-bit words"));
	CHECK(ContainsIssue(missing_assets.Validate(),
		"runtime_layout.asset_directory must name an existing directory"));
}
