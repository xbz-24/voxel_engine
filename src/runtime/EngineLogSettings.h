#pragma once

#include "LogRecord.h"

#include <filesystem>

namespace ve::engine
{
	struct RuntimeLogSettings
	{
		ve::log::Level minimum_level =
#if defined(NDEBUG)
			ve::log::Level::Info;
#else
			ve::log::Level::Debug;
#endif
		bool console_enabled = true;
		bool file_output_enabled = false;
		std::filesystem::path file_output_path;
	};
}
