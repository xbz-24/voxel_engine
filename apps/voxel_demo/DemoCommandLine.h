#pragma once

#include <span>
#include <string_view>

namespace voxel_demo
{
	struct DemoOptions
	{
		int smoke_frame_limit = 0;
		bool valid = true;
	};

	[[nodiscard]] DemoOptions ParseOptions(std::span<const std::string_view> arguments) noexcept;
}
