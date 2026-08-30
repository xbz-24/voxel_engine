#include "LoggerService.h"

#include "LogFormatter.h"

#include <chrono>
#include <thread>
#include <utility>

namespace ve::log
{
	void LoggerService::Write(Level level, std::string_view category,
		std::string_view message, SourceLocation source) noexcept
	{
		Write(level, category, message, std::span<const Field>{}, source);
	}

	void LoggerService::Write(Level level, std::string_view category,
		std::string_view message, std::span<const Field> fields,
		SourceLocation source) noexcept
	{
		try
		{
			const Record record{ level, category, message, source,
				std::chrono::system_clock::now(), std::this_thread::get_id(), fields };
			std::shared_ptr<Callback> callback;
			std::string formatted_record;
			{
				std::lock_guard<std::mutex> lock(mutex_);
				backend_.Write(record);
				callback = callback_;
				if (callback) formatted_record = FormatRecord(record);
			}
			if (callback) (*callback)(std::move(formatted_record));
		}
		catch (...)
		{
			// Logging and user-provided sinks must never break runtime cleanup.
		}
	}
}
