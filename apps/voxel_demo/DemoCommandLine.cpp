#include "DemoCommandLine.h"

#include <charconv>

namespace voxel_demo
{
	namespace
	{
		[[nodiscard]] bool TryParsePositiveInteger(std::string_view text, int& value) noexcept
		{
			constexpr int kMaximumFrameLimit = 100000;
			int parsed_value = 0;
			const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), parsed_value);
			if (error != std::errc{} || end != text.data() + text.size() ||
				parsed_value <= 0 || parsed_value > kMaximumFrameLimit) return false;
			value = parsed_value;
			return true;
		}
	}

	DemoOptions ParseOptions(std::span<const std::string_view> arguments) noexcept
	{
		DemoOptions options{};
		for (std::size_t index = 1; index < arguments.size(); ++index)
		{
			if (arguments[index] != "--smoke-frames" || index + 1 >= arguments.size() ||
				!TryParsePositiveInteger(arguments[++index], options.smoke_frame_limit))
			{
				options.valid = false;
				return options;
			}
		}
		return options;
	}
}
