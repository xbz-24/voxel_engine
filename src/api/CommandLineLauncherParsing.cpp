#include "CommandLineLauncher.h"

#include "CommandLineLauncherInternal.h"

namespace ve::launcher
{
	namespace
	{
		[[nodiscard]] bool TryReadValue(const CommandLineArguments& arguments,
			CommandLineArguments::size_type& argument_index,
			std::string_view& option_value) noexcept
		{
			if (argument_index + 1 >= arguments.size()) return false;
			option_value = arguments[++argument_index];
			return true;
		}

		[[nodiscard]] LaunchParseResult MarkInvalid(LaunchParseResult parse_result) noexcept
		{
			parse_result.ok = false;
			return parse_result;
		}
	}

	LaunchParseResult ParseLaunchArguments(const CommandLineArguments& arguments) noexcept
	{
		LaunchParseResult parse_result{};
		parse_result.options.launch_demo_suite = arguments.size() == 1;
		for (CommandLineArguments::size_type argument_index = 1; argument_index < arguments.size(); ++argument_index)
		{
			const std::string_view current_argument = arguments[argument_index];
			if (current_argument == "--demo")
			{
				if (!TryReadValue(arguments, argument_index, parse_result.options.selected_demo_name))
				{
					return MarkInvalid(parse_result);
				}
				parse_result.options.launch_demo_suite = IsDemoSuiteAlias(parse_result.options.selected_demo_name);
			}
			else if (current_argument == "--smoke-frames")
			{
				std::string_view frame_limit_text{};
				if (!TryReadValue(arguments, argument_index, frame_limit_text) ||
					!TryParseSmokeFrameLimit(frame_limit_text, parse_result.options.smoke_frame_limit))
				{
					return MarkInvalid(parse_result);
				}
			}
			else if (current_argument == "--all-demos") parse_result.options.launch_demo_suite = true;
			else return MarkInvalid(parse_result);
		}
		return parse_result;
	}
}
