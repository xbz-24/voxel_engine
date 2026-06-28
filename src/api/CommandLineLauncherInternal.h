#pragma once

#include <chrono>
#include <span>
#include <string_view>

namespace ve::launcher
{
	struct DemoProcess
	{
		std::string_view name;
		std::chrono::seconds delay_after_launch;
	};

	struct DemoLaunchRequest
	{
		std::string_view executable;
		std::string_view demo_name;
		int smoke_frames = 0;
	};

	[[nodiscard]] std::span<const DemoProcess> DemoSuite() noexcept;
	[[nodiscard]] bool IsAquaDemo(std::string_view demo) noexcept;
	[[nodiscard]] bool IsSponzaDemo(std::string_view demo) noexcept;
	[[nodiscard]] bool IsSuiteAlias(std::string_view demo) noexcept;
	[[nodiscard]] bool TryParsePositiveFrameCount(std::string_view text, int& value) noexcept;
}
