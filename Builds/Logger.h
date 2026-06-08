#pragma once

#include <string_view>

namespace ve::log
{
	enum class Level
	{
		Info,
		Warning,
		Error
	};

	/**
	 * Writes a message with the selected severity.
	 *
	 * @param level Severity used to prefix and route the message.
	 * @param message Text written to the log.
	 */
	void Write(Level level, std::string_view message);

	/**
	 * Writes an informational message.
	 *
	 * @param message Text written to the log.
	 */
	void Info(std::string_view message);

	/**
	 * Writes a warning message.
	 *
	 * @param message Text written to the log.
	 */
	void Warning(std::string_view message);

	/**
	 * Writes an error message.
	 *
	 * @param message Text written to the log.
	 */
	void Error(std::string_view message);
}

