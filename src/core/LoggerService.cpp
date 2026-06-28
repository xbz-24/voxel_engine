#include "LoggerService.h"

#include "LogFormatter.h"

#include <utility>

namespace ve::log
{
	LoggerService& LoggerService::Instance()
	{
		static LoggerService logger_service;
		return logger_service;
	}

	void LoggerService::SetMinimumLevel(Level level)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		backend_.SetMinimumLevel(level);
	}

	void LoggerService::ApplyConfiguration(const LoggerConfiguration& configuration)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		backend_.ApplyConfiguration(configuration);
	}

	Level LoggerService::MinimumLevel()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return backend_.MinimumLevel();
	}

	void LoggerService::SetConsoleEnabled(bool is_enabled)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		backend_.SetConsoleEnabled(is_enabled);
	}

	bool LoggerService::SetFileOutput(const std::filesystem::path& path)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return backend_.SetFileOutput(path);
	}

	void LoggerService::SetCallback(std::function<void(std::string)> callback)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		callback_ = std::move(callback);
	}

	void LoggerService::ClearFileOutput()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		backend_.ClearFileOutput();
	}

	void LoggerService::Write(Level level, std::string_view category, std::string_view message, SourceLocation source)
	{
		const Record record{ level, category, message, source, std::chrono::system_clock::now(), std::this_thread::get_id() };
		std::function<void(std::string)> callback;
		std::string formatted_record;
		{
			std::lock_guard<std::mutex> lock(mutex_);
			backend_.Write(record);
			callback = callback_;
			if (callback)
			{
				formatted_record = FormatRecord(record);
			}
		}
		if (callback)
		{
			callback(std::move(formatted_record));
		}
	}
}
