#include <doctest/doctest.h>

#include "DemoCommandLine.h"
#include "DemoConfig.h"

#include <array>
#include <initializer_list>
#include <span>
#include <string_view>

namespace
{
	void CheckInvalid(std::initializer_list<std::string_view> arguments)
	{
		const std::span<const std::string_view> argument_span{
			arguments.begin(), arguments.size() };
		CHECK_FALSE(voxel_demo::ParseOptions(argument_span).valid);
	}
}

TEST_CASE("voxel demo command line defaults to Vulkan")
{
	constexpr std::array<std::string_view, 1> arguments{{ "voxel_demo" }};
	const voxel_demo::DemoOptions options = voxel_demo::ParseOptions(arguments);

	CHECK(options.valid);
	CHECK(options.smoke_frame_limit == 0);
	CHECK(options.resize_smoke_frame == 0);
	CHECK(options.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(voxel_demo::CreateDemoConfig().StartupConfig().graphics_api ==
		voxel::GraphicsApi::Vulkan);
}

TEST_CASE("voxel demo command line selects each exposed graphics API")
{
	constexpr std::array<std::string_view, 7> vulkan_arguments{{
		"voxel_demo", "--graphics-api", "vulkan",
		"--smoke-frames", "3", "--resize-smoke-frame", "2"
	}};
	constexpr std::array<std::string_view, 5> opengl_arguments{{
		"voxel_demo", "--smoke-frames", "2",
		"--graphics-api", "opengl"
	}};
	const voxel_demo::DemoOptions vulkan = voxel_demo::ParseOptions(vulkan_arguments);
	const voxel_demo::DemoOptions opengl = voxel_demo::ParseOptions(opengl_arguments);

	CHECK(vulkan.valid);
	CHECK(vulkan.smoke_frame_limit == 3);
	CHECK(vulkan.resize_smoke_frame == 2);
	CHECK(vulkan.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(opengl.valid);
	CHECK(opengl.smoke_frame_limit == 2);
	CHECK(opengl.graphics_api == voxel::GraphicsApi::OpenGLCompatibility);
	CHECK(voxel_demo::CreateDemoConfig(opengl.graphics_api).StartupConfig().graphics_api ==
		voxel::GraphicsApi::OpenGLCompatibility);
}

TEST_CASE("voxel demo command line rejects malformed options")
{
	CheckInvalid({ "voxel_demo", "--graphics-api" });
	CheckInvalid({ "voxel_demo", "--graphics-api", "directx12" });
	CheckInvalid({ "voxel_demo", "--smoke-frames", "0" });
	CheckInvalid({ "voxel_demo", "--renderer", "vulkan" });
	CheckInvalid({ "voxel_demo", "--resize-smoke-frame", "1" });
	CheckInvalid({ "voxel_demo", "--smoke-frames", "3", "--resize-smoke-frame", "0" });
	CheckInvalid({ "voxel_demo", "--smoke-frames", "3",
		"--resize-smoke-frame", "3" });
	CheckInvalid({ "voxel_demo", "--smoke-frames", "3",
		"--resize-smoke-frame", "4" });
	CheckInvalid({ "voxel_demo", "--graphics-api", "opengl",
		"--smoke-frames", "3", "--resize-smoke-frame", "1" });
}
