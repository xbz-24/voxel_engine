#pragma once

#include <chrono>
#include <string_view>
#include <thread>

namespace ve::log
{
	/// Defines the severity order used by filtering and sinks.
	enum class Level
	{
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};

	/// Describes the call site that produced a log message.
	struct SourceLocation
	{
		std::string_view file;
		std::string_view function;
		int line = 0;
	};

	/// Stores all structured data for one log event.
	struct Record
	{
		Level level;
		std::string_view message;
		SourceLocation source;
		std::chrono::system_clock::time_point timestamp;
		std::thread::id threadId;
	};
}
