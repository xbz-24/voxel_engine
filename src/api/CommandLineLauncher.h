#pragma once

#include <string_view>

namespace ve::launcher
{
	struct LaunchOptions
	{
		std::string_view demo_name = "desert";
		int smoke_frames = 0;
		bool launch_all = false;
	};

	struct LaunchParseResult
	{
		LaunchOptions options{};
		bool ok = true;
	};

	[[nodiscard]] LaunchParseResult ParseLaunchArguments(int argc, char** argv) noexcept;
	[[nodiscard]] int RunCommandLine(int argc, char** argv);
}
