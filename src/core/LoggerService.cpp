#include "LoggerService.h"

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

	bool LoggerService::ApplyConfiguration(const LoggerConfiguration& configuration)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return backend_.ApplyConfiguration(configuration);
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
		std::shared_ptr<Callback> replacement;
		if (callback) replacement = std::make_shared<Callback>(std::move(callback));
		{
			std::lock_guard<std::mutex> lock(mutex_);
			callback_.swap(replacement);
		}
	}

	void LoggerService::ClearFileOutput()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		backend_.ClearFileOutput();
	}

	void LoggerService::ResetRuntimeState()
	{
		std::shared_ptr<Callback> detached_callback;
		{
			std::lock_guard<std::mutex> lock(mutex_);
			detached_callback.swap(callback_);
			backend_.ResetRuntimeState();
		}
	}

}
