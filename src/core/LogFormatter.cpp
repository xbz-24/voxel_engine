#include "LogFormatter.h"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string_view>

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

	namespace
	{
		bool NeedsQuoting(std::string_view value) noexcept
		{
			if (value.empty()) return true;
			for (const char character : value)
			{
				const unsigned char unsigned_character = static_cast<unsigned char>(character);
				if (std::isspace(unsigned_character) || character == '"' || character == '\\')
				{
					return true;
				}
			}
			return false;
		}

		void WriteQuotedValue(std::ostream& stream, std::string_view value)
		{
			stream << '"';
			for (const char character : value)
			{
				if (character == '"' || character == '\\') stream << '\\';
				stream << character;
			}
			stream << '"';
		}

		void WriteFields(std::ostream& stream, std::span<const Field> fields)
		{
			for (const Field& field : fields)
			{
				if (field.name.empty()) continue;
				stream << ' ' << field.name << '=';
				if (NeedsQuoting(field.value))
				{
					WriteQuotedValue(stream, field.value);
				}
				else
				{
					stream << field.value;
				}
			}
		}
	}

	std::string FormatMessageWithFields(const Record& record)
	{
		std::ostringstream stream;
		stream << record.message;
		WriteFields(stream, record.fields);
		return stream.str();
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
		stream << "[thread " << record.threadId << "] " << FormatMessageWithFields(record);
		if (!record.source.file.empty())
		{
			stream << " (" << record.source.file << ':' << record.source.line << ' ' << record.source.function << ')';
		}
		return stream.str();
	}
}
