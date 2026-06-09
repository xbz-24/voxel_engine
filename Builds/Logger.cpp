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
		LoggerState& state = State();
		std::lock_guard<std::mutex> lock(state.mutex);
		state.minimumLevel = level;
	}

	void ApplyConfiguration(const LoggerConfiguration& configuration)
	{
		LoggerState& state = State();
		std::lock_guard<std::mutex> lock(state.mutex);
		state.minimumLevel = configuration.minimumLevel;
		state.consoleEnabled = configuration.consoleEnabled;
		state.file.close();
		if (configuration.fileOutputPath)
		{
			state.file.open(*configuration.fileOutputPath, std::ios::app);
		}
	}

	Level MinimumLevel()
	{
		LoggerState& state = State();
		std::lock_guard<std::mutex> lock(state.mutex);
		return state.minimumLevel;
	}

	void SetConsoleEnabled(bool isEnabled)
	{
		LoggerState& state = State();
		std::lock_guard<std::mutex> lock(state.mutex);
		state.consoleEnabled = isEnabled;
	}

	bool SetFileOutput(const std::filesystem::path& path)
	{
		LoggerState& state = State();
		std::lock_guard<std::mutex> lock(state.mutex);
		state.file.close();
		state.file.open(path, std::ios::app);
		return state.file.is_open();
	}

	void ClearFileOutput()
	{
		LoggerState& state = State();
		std::lock_guard<std::mutex> lock(state.mutex);
		state.file.close();
	}

	void Write(Level level, std::string_view message, SourceLocation source)
	{
		Write(level, category::General, message, source);
	}

	void Write(Level level, std::string_view category, std::string_view message, SourceLocation source)
	{
		LoggerState& state = State();
		std::lock_guard<std::mutex> lock(state.mutex);
		if (!IsEnabled(level, state.minimumLevel)) return;
		Record record{ level, category, message, source, std::chrono::system_clock::now(), std::this_thread::get_id() };
		const std::string line = FormatRecord(record);
		if (state.consoleEnabled) (level >= Level::Error ? std::cerr : std::cout) << line << '\n';
		if (state.file.is_open()) state.file << line << '\n';
	}

	void Trace(std::string_view message, SourceLocation source) { Write(Level::Trace, message, source); }
	void Debug(std::string_view message, SourceLocation source) { Write(Level::Debug, message, source); }
	void Info(std::string_view message, SourceLocation source) { Write(Level::Info, message, source); }
	void Warning(std::string_view message, SourceLocation source) { Write(Level::Warning, message, source); }
	void Error(std::string_view message, SourceLocation source) { Write(Level::Error, message, source); }
	void Fatal(std::string_view message, SourceLocation source) { Write(Level::Fatal, message, source); }
}
