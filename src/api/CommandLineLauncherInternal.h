#pragma once

#include <chrono>
#include <span>
#include <string_view>

namespace ve::launcher
{
	struct DemoProcess
	{
		std::string_view demo_name;
		std::chrono::seconds pause_after_launch;
	};

	struct DemoLaunchRequest
	{
		std::string_view executable_path;
		std::string_view requested_demo_name;
		int smoke_frame_limit = 0;
	};

	[[nodiscard]] std::span<const DemoProcess> DemoSuite() noexcept;
	[[nodiscard]] bool IsAquaDemoAlias(std::string_view requested_demo_name) noexcept;
	[[nodiscard]] bool IsSponzaDemoAlias(std::string_view requested_demo_name) noexcept;
	[[nodiscard]] bool IsDemoSuiteAlias(std::string_view requested_demo_name) noexcept;
	[[nodiscard]] bool TryParseSmokeFrameLimit(std::string_view frame_limit_text, int& parsed_frame_limit) noexcept;
}
