#pragma once

#include <voxel/GraphicsConfig.h>

#include <span>
#include <string_view>

namespace voxel_demo
{
	struct DemoOptions
	{
		int smoke_frame_limit = 0;
		voxel::GraphicsApi graphics_api = voxel::GraphicsApi::Vulkan;
		bool valid = true;
	};

	[[nodiscard]] DemoOptions ParseOptions(std::span<const std::string_view> arguments) noexcept;
}
