#include "CommandLineLauncher.h"

#include "CommandLineLauncherInternal.h"

#include <array>

namespace ve::launcher
{
	std::span<const DemoProcess> DemoSuite() noexcept
	{
		static constexpr std::array<DemoProcess, 3> kDemoSuite{ {
			DemoProcess{ "aqua", std::chrono::seconds{ 5 } },
			DemoProcess{ "sponza", std::chrono::seconds{ 8 } },
			DemoProcess{ "voxeldemo", std::chrono::seconds{ 0 } }
		} };
		return kDemoSuite;
	}

	bool IsAquaDemo(std::string_view demo) noexcept
	{
		return demo == "aqua" || demo == "konosuba" || demo == "konosuba-aqua";
	}

	bool IsSponzaDemo(std::string_view demo) noexcept
	{
		return demo == "sponza" || demo == "atrium" || demo == "sponza-atrium" ||
			demo == "coliseo" || demo == "coliseum";
	}

	bool IsSuiteAlias(std::string_view demo) noexcept
	{
		return demo == "all" || demo == "all-demos" || demo == "demo-all";
	}

	bool TryParsePositiveFrameCount(std::string_view text, int& value) noexcept
	{
		constexpr int kMaxFrameCount = 100000;
		if (text.empty()) return false;

		int parsed = 0;
		for (const char ch : text)
		{
			if (ch < '0' || ch > '9') return false;
			const int digit = ch - '0';
			if (parsed > (kMaxFrameCount - digit) / 10) return false;
			parsed = (parsed * 10) + digit;
		}

		if (parsed <= 0) return false;
		value = parsed;
		return true;
	}

	namespace
	{
		[[nodiscard]] bool TryReadValue(const CommandLineArguments& arguments,
			CommandLineArguments::size_type& index,
			std::string_view& value) noexcept
		{
			if (index + 1 >= arguments.size()) return false;
			value = arguments[++index];
			return true;
		}

		[[nodiscard]] LaunchParseResult Invalid(LaunchParseResult result) noexcept
		{
			result.ok = false;
			return result;
		}
	}

	LaunchParseResult ParseLaunchArguments(const CommandLineArguments& arguments) noexcept
	{
		LaunchParseResult result{};
		result.options.launch_all = arguments.size() == 1;
		for (CommandLineArguments::size_type index = 1; index < arguments.size(); ++index)
		{
			const std::string_view arg = arguments[index];
			if (arg == "--demo")
			{
				if (!TryReadValue(arguments, index, result.options.demo_name)) return Invalid(result);
				result.options.launch_all = IsSuiteAlias(result.options.demo_name);
			}
			else if (arg == "--smoke-frames")
			{
				std::string_view frames{};
				if (!TryReadValue(arguments, index, frames) ||
					!TryParsePositiveFrameCount(frames, result.options.smoke_frames)) return Invalid(result);
			}
			else if (arg == "--all-demos") result.options.launch_all = true;
			else return Invalid(result);
		}
		return result;
	}
}
