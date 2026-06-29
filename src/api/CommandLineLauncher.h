#pragma once

#include <string_view>
#include <vector>

namespace ve::launcher
{
	using CommandLineArguments = std::vector<std::string_view>;

	struct LaunchOptions
	{
		std::string_view selected_demo_name = "desert";
		int smoke_frame_limit = 0;
		bool launch_demo_suite = false;
	};

	struct LaunchParseResult
	{
		LaunchOptions options{};
		bool ok = true;
	};

	[[nodiscard]] LaunchParseResult ParseLaunchArguments(const CommandLineArguments& arguments) noexcept;
	[[nodiscard]] int RunCommandLine(const CommandLineArguments& arguments);
}
