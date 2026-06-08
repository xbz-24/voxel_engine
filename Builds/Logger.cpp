#include "Logger.h"

#include <iostream>

namespace
{
	/**
	 * Converts a log level into a readable prefix.
	 *
	 * @param level Severity to convert.
	 * @return Static prefix string for the severity.
	 */
	const char* PrefixFor(ve::log::Level level)
	{
		switch (level)
		{
		case ve::log::Level::Info:
			return "[info] ";
		case ve::log::Level::Warning:
			return "[warn] ";
		case ve::log::Level::Error:
			return "[error] ";
		default:
			return "[log] ";
		}
	}
}

namespace ve::log
{
	void Write(Level level, std::string_view message)
	{
		std::ostream& stream = level == Level::Error ? std::cerr : std::cout;
		stream << PrefixFor(level) << message << '\n';
	}

	void Info(std::string_view message)
	{
		Write(Level::Info, message);
	}

	void Warning(std::string_view message)
	{
		Write(Level::Warning, message);
	}

	void Error(std::string_view message)
	{
		Write(Level::Error, message);
	}
}

