#pragma once

#include "LogRecord.h"

#include <string>

namespace ve::log
{
	/// Returns a stable uppercase name for a severity level.
	const char* LevelName(Level level) noexcept;

	/// Formats a record into a single human-readable log line.
	std::string FormatRecord(const Record& record);
}
