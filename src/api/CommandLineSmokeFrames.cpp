#include "CommandLineLauncherInternal.h"

namespace ve::launcher
{
	bool TryParseSmokeFrameLimit(std::string_view frame_limit_text, int& parsed_frame_limit) noexcept
	{
		constexpr int kMaxFrameCount = 100000;
		if (frame_limit_text.empty()) return false;

		int frame_limit_value = 0;
		for (const char digit_character : frame_limit_text)
		{
			if (digit_character < '0' || digit_character > '9') return false;
			const int digit_value = digit_character - '0';
			if (frame_limit_value > (kMaxFrameCount - digit_value) / 10) return false;
			frame_limit_value = (frame_limit_value * 10) + digit_value;
		}

		if (frame_limit_value <= 0) return false;
		parsed_frame_limit = frame_limit_value;
		return true;
	}
}
