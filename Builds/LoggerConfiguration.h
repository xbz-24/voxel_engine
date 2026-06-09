#pragma once

#include "LogRecord.h"

#include <filesystem>
#include <optional>

namespace ve::log
{
	struct LoggerConfiguration
	{
		Level minimumLevel = Level::Info;
		bool consoleEnabled = true;
		std::optional<std::filesystem::path> fileOutputPath;
	};
}
