#pragma once

#include "LogCategory.h"
#include "LogRecord.h"
#include "LoggerConfiguration.h"

#include <filesystem>
#include <functional>
#include <string>
#include <string_view>

namespace ve::log
{
	/**
	 * Updates the minimum severity accepted by the logger.
	 *
	 * @param level Lowest severity that should be written.
	 */
	void SetMinimumLevel(Level level);

	/**
	 * Applies logger settings in one call.
	 *
	 * @param configuration Minimum level plus enabled sinks.
	 */
	void ApplyConfiguration(const LoggerConfiguration& configuration);

	/**
	 * Returns the current minimum accepted severity.
	 *
	 * @return Lowest severity currently accepted by the logger.
	 */
	Level MinimumLevel();

	/**
	 * Enables or disables console output.
	 *
	 * @param isEnabled True to write records to stdout/stderr.
	 */
	void SetConsoleEnabled(bool isEnabled);

	/**
	 * Opens an append-only file sink for future log records.
	 *
	 * @param path File path that receives formatted log records.
	 * @return True when the file sink was opened successfully.
	 */
	bool SetFileOutput(const std::filesystem::path& path);

	/** @param callback Optional sink receiving formatted log lines after built-in sinks. */
	void SetCallback(std::function<void(std::string)> callback);

	/**
	 * Closes the file sink when one is active.
	 */
	void ClearFileOutput();

	/**
	 * Writes a structured message with a selected severity.
	 *
	 * @param level Severity assigned to the record.
	 * @param message Human-readable message body.
	 * @param source Optional source location captured by logging macros.
	 */
	void Write(Level level, std::string_view message, SourceLocation source = {});

	/**
	 * Writes a structured category message with a selected severity.
	 *
	 * @param level Severity assigned to the record.
	 * @param category Subsystem or feature that produced the record.
	 * @param message Human-readable message body.
	 * @param source Optional source location captured by logging macros.
	 */
	void Write(Level level, std::string_view category, std::string_view message, SourceLocation source = {});

	/**
	 * Writes a trace-level message.
	 *
	 * @param message Human-readable message body.
	 * @param source Optional source location captured by logging macros.
	 */
	void Trace(std::string_view message, SourceLocation source = {});

	/**
	 * Writes a debug-level message.
	 *
	 * @param message Human-readable message body.
	 * @param source Optional source location captured by logging macros.
	 */
	void Debug(std::string_view message, SourceLocation source = {});

	/**
	 * Writes an info-level message.
	 *
	 * @param message Human-readable message body.
	 * @param source Optional source location captured by logging macros.
	 */
	void Info(std::string_view message, SourceLocation source = {});

	/**
	 * Writes a warning-level message.
	 *
	 * @param message Human-readable message body.
	 * @param source Optional source location captured by logging macros.
	 */
	void Warning(std::string_view message, SourceLocation source = {});

	/**
	 * Writes an error-level message.
	 *
	 * @param message Human-readable message body.
	 * @param source Optional source location captured by logging macros.
	 */
	void Error(std::string_view message, SourceLocation source = {});

	/**
	 * Writes a fatal-level message.
	 *
	 * @param message Human-readable message body.
	 * @param source Optional source location captured by logging macros.
	 */
	void Fatal(std::string_view message, SourceLocation source = {});
}

#include "LogMacros.h"
