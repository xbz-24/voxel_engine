#include <doctest/doctest.h>

#include "DemoCommandLine.h"
#include "DemoConfig.h"

#include <array>
#include <string_view>

TEST_CASE("voxel demo command line defaults to Vulkan")
{
	constexpr std::array<std::string_view, 1> arguments{{ "voxel_demo" }};
	const voxel_demo::DemoOptions options = voxel_demo::ParseOptions(arguments);

	CHECK(options.valid);
	CHECK(options.smoke_frame_limit == 0);
	CHECK(options.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(voxel_demo::CreateDemoConfig().StartupConfig().graphics_api ==
		voxel::GraphicsApi::Vulkan);
}

TEST_CASE("voxel demo command line selects each exposed graphics API")
{
	constexpr std::array<std::string_view, 5> vulkan_arguments{{
		"voxel_demo", "--graphics-api", "vulkan",
		"--smoke-frames", "3"
	}};
	constexpr std::array<std::string_view, 5> opengl_arguments{{
		"voxel_demo", "--smoke-frames", "2",
		"--graphics-api", "opengl"
	}};
	const voxel_demo::DemoOptions vulkan = voxel_demo::ParseOptions(vulkan_arguments);
	const voxel_demo::DemoOptions opengl = voxel_demo::ParseOptions(opengl_arguments);

	CHECK(vulkan.valid);
	CHECK(vulkan.smoke_frame_limit == 3);
	CHECK(vulkan.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(opengl.valid);
	CHECK(opengl.smoke_frame_limit == 2);
	CHECK(opengl.graphics_api == voxel::GraphicsApi::OpenGLCompatibility);
	CHECK(voxel_demo::CreateDemoConfig(opengl.graphics_api).StartupConfig().graphics_api ==
		voxel::GraphicsApi::OpenGLCompatibility);
}

TEST_CASE("voxel demo command line rejects malformed options")
{
	constexpr std::array<std::string_view, 2> missing_value{{
		"voxel_demo", "--graphics-api"
	}};
	constexpr std::array<std::string_view, 3> unknown_api{{
		"voxel_demo", "--graphics-api", "directx12"
	}};
	constexpr std::array<std::string_view, 3> invalid_frame_count{{
		"voxel_demo", "--smoke-frames", "0"
	}};
	constexpr std::array<std::string_view, 3> unknown_option{{
		"voxel_demo", "--renderer", "vulkan"
	}};

	CHECK(!voxel_demo::ParseOptions(missing_value).valid);
	CHECK(!voxel_demo::ParseOptions(unknown_api).valid);
	CHECK(!voxel_demo::ParseOptions(invalid_frame_count).valid);
	CHECK(!voxel_demo::ParseOptions(unknown_option).valid);
}
