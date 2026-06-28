#include "Logger.h"

#include "LoggerService.h"

#include <utility>

namespace ve::log
{
	void SetMinimumLevel(Level level) { LoggerService::Instance().SetMinimumLevel(level); }

	void ApplyConfiguration(const LoggerConfiguration& configuration) { LoggerService::Instance().ApplyConfiguration(configuration); }

	Level MinimumLevel() { return LoggerService::Instance().MinimumLevel(); }

	void SetConsoleEnabled(bool is_enabled) { LoggerService::Instance().SetConsoleEnabled(is_enabled); }

	bool SetFileOutput(const std::filesystem::path& path) { return LoggerService::Instance().SetFileOutput(path); }

	void SetCallback(std::function<void(std::string)> callback) { LoggerService::Instance().SetCallback(std::move(callback)); }

	void ClearFileOutput() { LoggerService::Instance().ClearFileOutput(); }

	void Write(Level level, std::string_view message, SourceLocation source)
	{
		LoggerService::Instance().Write(level, category::General, message, source);
	}

	void Write(Level level, std::string_view category, std::string_view message, SourceLocation source)
	{
		LoggerService::Instance().Write(level, category, message, source);
	}

	void Trace(std::string_view message, SourceLocation source) { Write(Level::Trace, message, source); }
	void Debug(std::string_view message, SourceLocation source) { Write(Level::Debug, message, source); }
	void Info(std::string_view message, SourceLocation source) { Write(Level::Info, message, source); }
	void Warning(std::string_view message, SourceLocation source) { Write(Level::Warning, message, source); }
	void Error(std::string_view message, SourceLocation source) { Write(Level::Error, message, source); }
	void Fatal(std::string_view message, SourceLocation source) { Write(Level::Fatal, message, source); }
}
