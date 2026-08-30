#include <doctest/doctest.h>

#include "DemoCommandLine.h"

#include <array>
#include <string_view>

TEST_CASE("voxel demo command line captures an explicit Vulkan runtime layout")
{
	constexpr std::array<std::string_view, 7> arguments{{
		"voxel_demo", "--graphics-api", "vulkan",
		"--asset-directory", "content/assets",
		"--vulkan-shader-directory", "content/shaders"
	}};
	const voxel_demo::DemoOptions options = voxel_demo::ParseOptions(arguments);

	CHECK(options.valid);
	CHECK(options.asset_directory == "content/assets");
	CHECK(options.vulkan_shader_directory == "content/shaders");
}

TEST_CASE("voxel demo runtime layout options respect backend requirements")
{
	constexpr std::array<std::string_view, 5> opengl_assets{{
		"voxel_demo", "--graphics-api", "opengl",
		"--asset-directory", "content/assets"
	}};
	constexpr std::array<std::string_view, 3> vulkan_assets_only{{
		"voxel_demo", "--asset-directory", "content/assets"
	}};
	constexpr std::array<std::string_view, 3> shaders_only{{
		"voxel_demo", "--vulkan-shader-directory", "content/shaders"
	}};

	CHECK(voxel_demo::ParseOptions(opengl_assets).valid);
	CHECK_FALSE(voxel_demo::ParseOptions(vulkan_assets_only).valid);
	CHECK_FALSE(voxel_demo::ParseOptions(shaders_only).valid);
}
