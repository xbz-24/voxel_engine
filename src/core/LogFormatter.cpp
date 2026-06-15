#include "LogFormatter.h"

#include <iomanip>
#include <sstream>

namespace ve::log
{
	const char* LevelName(Level level) noexcept
	{
		switch (level)
		{
		case Level::Trace: return "TRACE";
		case Level::Debug: return "DEBUG";
		case Level::Info: return "INFO";
		case Level::Warning: return "WARN";
		case Level::Error: return "ERROR";
		case Level::Fatal: return "FATAL";
		}
		return "LOG";
	}

	std::string FormatRecord(const Record& record)
	{
		const auto time = std::chrono::system_clock::to_time_t(record.timestamp);
		std::tm localTime{};
		localtime_s(&localTime, &time);

		std::ostringstream stream;
		stream << std::put_time(&localTime, "%H:%M:%S") << " [" << LevelName(record.level) << "] ";
		if (!record.category.empty())
		{
			stream << '[' << record.category << "] ";
		}
		stream << "[thread " << record.threadId << "] " << record.message;
		if (!record.source.file.empty())
		{
			stream << " (" << record.source.file << ':' << record.source.line << ' ' << record.source.function << ')';
		}
		return stream.str();
	}
}
