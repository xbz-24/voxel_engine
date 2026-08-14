#include <doctest/doctest.h>

#include <voxel/EngineConfig.h>

#include "EngineRuntimeBridge.h"

#include <filesystem>

TEST_CASE("runtime layout translation owns exact filesystem paths")
{
	voxel::EngineConfig config = voxel::EngineConfig::Default().WithRuntimeLayout(
		voxel::RuntimeLayout{}.AssetsAt("exact/assets").VulkanShadersAt("exact/shaders"));
	const ve::engine::EngineCreateInfo translated =
		voxel::detail::DefaultEngineConfigTranslator().Translate(config);
	const std::filesystem::path expected_assets =
		std::filesystem::absolute("exact/assets").lexically_normal();
	const std::filesystem::path expected_shaders =
		std::filesystem::absolute("exact/shaders").lexically_normal();

	REQUIRE(translated.asset_directory.has_value());
	REQUIRE(translated.vulkan_shader_directory.has_value());
	CHECK(*translated.asset_directory == expected_assets);
	CHECK(*translated.vulkan_shader_directory == expected_shaders);

	config.runtime_layout->asset_directory = "changed/assets";
	config.runtime_layout->vulkan_shader_directory = "changed/shaders";
	CHECK(*translated.asset_directory == expected_assets);
	CHECK(*translated.vulkan_shader_directory == expected_shaders);
}

TEST_CASE("discovered runtime layout leaves internal overrides disengaged")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default().UseDiscoveredRuntimeLayout();
	const ve::engine::EngineCreateInfo translated =
		voxel::detail::DefaultEngineConfigTranslator().Translate(config);

	CHECK(!translated.asset_directory.has_value());
	CHECK(!translated.vulkan_shader_directory.has_value());
}
