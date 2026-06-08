#pragma once

#include "LogRecord.h"

#include <filesystem>
#include <string_view>

namespace ve::log
{
	/// Updates the minimum severity accepted by the logger.
	void SetMinimumLevel(Level level);

	/// Returns the current minimum accepted severity.
	Level MinimumLevel();

	/// Enables or disables console output.
	void SetConsoleEnabled(bool isEnabled);

	/// Opens an append-only file sink for future log records.
	bool SetFileOutput(const std::filesystem::path& path);

	/// Closes the file sink when one is active.
	void ClearFileOutput();

	/// Writes a structured message with a selected severity.
	void Write(Level level, std::string_view message, SourceLocation source = {});

	/// Writes a trace-level diagnostic message.
	void Trace(std::string_view message, SourceLocation source = {});

	/// Writes a debug-level diagnostic message.
	void Debug(std::string_view message, SourceLocation source = {});

	/// Writes an informational message.
	void Info(std::string_view message, SourceLocation source = {});

	/// Writes a warning message.
	void Warning(std::string_view message, SourceLocation source = {});

	/// Writes an error message.
	void Error(std::string_view message, SourceLocation source = {});

	/// Writes a fatal message.
	void Fatal(std::string_view message, SourceLocation source = {});
}

#define VE_LOG_TRACE(message) ::ve::log::Trace((message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_DEBUG(message) ::ve::log::Debug((message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_INFO(message) ::ve::log::Info((message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_WARNING(message) ::ve::log::Warning((message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_ERROR(message) ::ve::log::Error((message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_FATAL(message) ::ve::log::Fatal((message), { __FILE__, __FUNCTION__, __LINE__ })
