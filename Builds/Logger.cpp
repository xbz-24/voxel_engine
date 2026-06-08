#include "Logger.h"

#include "LogFormatter.h"

#include <fstream>
#include <iostream>
#include <mutex>

namespace
{
	struct LoggerState
	{
		std::mutex mutex;
		ve::log::Level minimumLevel = ve::log::Level::Info;
		bool consoleEnabled = true;
		std::ofstream file;
	};

	/// Returns the process-wide logger state.
	LoggerState& State()
	{
		static LoggerState state;
		return state;
	}

	/// Checks whether a severity passes the current filter.
	bool IsEnabled(ve::log::Level level, ve::log::Level minimum)
	{
		return static_cast<int>(level) >= static_cast<int>(minimum);
	}
}

namespace ve::log
{
	void SetMinimumLevel(Level level)
	{
		std::lock_guard<std::mutex> lock(State().mutex);
		State().minimumLevel = level;
	}

	Level MinimumLevel()
	{
		std::lock_guard<std::mutex> lock(State().mutex);
		return State().minimumLevel;
	}

	void SetConsoleEnabled(bool isEnabled)
	{
		std::lock_guard<std::mutex> lock(State().mutex);
		State().consoleEnabled = isEnabled;
	}

	bool SetFileOutput(const std::filesystem::path& path)
	{
		std::lock_guard<std::mutex> lock(State().mutex);
		State().file.close();
		State().file.open(path, std::ios::app);
		return State().file.is_open();
	}

	void ClearFileOutput()
	{
		std::lock_guard<std::mutex> lock(State().mutex);
		State().file.close();
	}

	void Write(Level level, std::string_view message, SourceLocation source)
	{
		Record record{ level, message, source, std::chrono::system_clock::now(), std::this_thread::get_id() };
		const std::string line = FormatRecord(record);
		std::lock_guard<std::mutex> lock(State().mutex);
		if (!IsEnabled(level, State().minimumLevel)) return;
		if (State().consoleEnabled) (level >= Level::Error ? std::cerr : std::cout) << line << '\n';
		if (State().file.is_open()) State().file << line << '\n';
	}

	void Trace(std::string_view message, SourceLocation source) { Write(Level::Trace, message, source); }
	void Debug(std::string_view message, SourceLocation source) { Write(Level::Debug, message, source); }
	void Info(std::string_view message, SourceLocation source) { Write(Level::Info, message, source); }
	void Warning(std::string_view message, SourceLocation source) { Write(Level::Warning, message, source); }
	void Error(std::string_view message, SourceLocation source) { Write(Level::Error, message, source); }
	void Fatal(std::string_view message, SourceLocation source) { Write(Level::Fatal, message, source); }
}
