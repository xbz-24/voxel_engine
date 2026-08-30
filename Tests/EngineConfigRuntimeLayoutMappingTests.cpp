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

TEST_CASE("runtime layout translation preserves native Unicode paths")
{
#if defined(_WIN32)
	const std::filesystem::path native_root{ L"layout with spaces \u00e1\u6f22" };
#else
	const std::filesystem::path native_root{ u8"layout with spaces \u00e1\u6f22" };
#endif
	const std::filesystem::path assets = native_root / "assets";
	const std::filesystem::path shaders = native_root / "shaders";
	const voxel::EngineConfig config = voxel::EngineConfig::Default().WithRuntimeLayout(
		voxel::RuntimeLayout{}.AssetsAt(assets).VulkanShadersAt(shaders));
	const ve::engine::EngineCreateInfo translated =
		voxel::detail::DefaultEngineConfigTranslator().Translate(config);

	REQUIRE(translated.asset_directory.has_value());
	REQUIRE(translated.vulkan_shader_directory.has_value());
	CHECK(*translated.asset_directory == std::filesystem::absolute(assets).lexically_normal());
	CHECK(*translated.vulkan_shader_directory ==
		std::filesystem::absolute(shaders).lexically_normal());
}
