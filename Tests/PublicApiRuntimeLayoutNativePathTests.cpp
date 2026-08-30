#include <doctest/doctest.h>

#include <voxel/EngineConfig.h>

#include "RuntimeLayoutTestSupport.h"

#include <filesystem>

TEST_CASE("public runtime layout preserves native paths with spaces and Unicode")
{
	ve::tests::TemporaryRuntimeLayout files{ "native_paths" };
	files.WriteValidShaderBundle();
#if defined(_WIN32)
	const std::filesystem::path asset_directory = files.Assets().parent_path() /
		std::filesystem::path{ L"assets with spaces \u00e1\u6f22" };
	const std::filesystem::path shader_directory = files.Shaders().parent_path() /
		std::filesystem::path{ L"shaders with spaces \u00e1\u6f22" };
#else
	const std::filesystem::path asset_directory = files.Assets().parent_path() /
		std::filesystem::path{ u8"assets with spaces \u00e1\u6f22" };
	const std::filesystem::path shader_directory = files.Shaders().parent_path() /
		std::filesystem::path{ u8"shaders with spaces \u00e1\u6f22" };
#endif
	std::filesystem::rename(files.Assets(), asset_directory);
	std::filesystem::rename(files.Shaders(), shader_directory);

	const voxel::EngineConfig config = voxel::EngineConfig::Default().WithRuntimeLayout(
		voxel::RuntimeLayout{}.AssetsAt(asset_directory).VulkanShadersAt(shader_directory));
	const voxel::EngineStartupConfig startup = config.StartupConfig();

	CHECK(config.Validate().empty());
	REQUIRE(startup.runtime_layout.has_value());
	CHECK(startup.runtime_layout->asset_directory == asset_directory);
	CHECK(startup.runtime_layout->vulkan_shader_directory == shader_directory);
}
