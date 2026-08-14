#include "Logger.h"

#include "LoggerService.h"

#include <utility>

namespace ve::log
{
	void SetMinimumLevel(Level level) { LoggerService::Instance().SetMinimumLevel(level); }

	bool ApplyConfiguration(const LoggerConfiguration& configuration)
	{ return LoggerService::Instance().ApplyConfiguration(configuration); }

	Level MinimumLevel() { return LoggerService::Instance().MinimumLevel(); }

	void SetConsoleEnabled(bool is_enabled) { LoggerService::Instance().SetConsoleEnabled(is_enabled); }

	bool SetFileOutput(const std::filesystem::path& path) { return LoggerService::Instance().SetFileOutput(path); }

	void SetCallback(std::function<void(std::string)> callback) { LoggerService::Instance().SetCallback(std::move(callback)); }

	void ClearFileOutput() { LoggerService::Instance().ClearFileOutput(); }

	void ResetRuntimeState() { LoggerService::Instance().ResetRuntimeState(); }

	void Write(Level level, std::string_view message, SourceLocation source) noexcept
	{
		try { LoggerService::Instance().Write(level, category::General, message, source); }
		catch (...) {}
	}

	void Write(Level level, std::string_view category,
		std::string_view message, SourceLocation source) noexcept
	{
		try { LoggerService::Instance().Write(level, category, message, source); }
		catch (...) {}
	}

	void Write(Level level, std::string_view message,
		std::span<const Field> fields, SourceLocation source) noexcept
	{
		try { LoggerService::Instance().Write(level, category::General, message, fields, source); }
		catch (...) {}
	}

	void Write(
		Level level,
		std::string_view category,
		std::string_view message,
		std::span<const Field> fields,
		SourceLocation source) noexcept
	{
		try { LoggerService::Instance().Write(level, category, message, fields, source); }
		catch (...) {}
	}

	void Trace(std::string_view message, SourceLocation source) noexcept { Write(Level::Trace, message, source); }
	void Debug(std::string_view message, SourceLocation source) noexcept { Write(Level::Debug, message, source); }
	void Info(std::string_view message, SourceLocation source) noexcept { Write(Level::Info, message, source); }
	void Warning(std::string_view message, SourceLocation source) noexcept { Write(Level::Warning, message, source); }
	void Error(std::string_view message, SourceLocation source) noexcept { Write(Level::Error, message, source); }
	void Fatal(std::string_view message, SourceLocation source) noexcept { Write(Level::Fatal, message, source); }
}
