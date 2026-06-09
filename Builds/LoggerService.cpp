#include "LoggerService.h"

#include "LogFormatter.h"

#include <iostream>

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
		minimum_level_ = level;
	}

	void LoggerService::ApplyConfiguration(const LoggerConfiguration& configuration)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		minimum_level_ = configuration.minimumLevel;
		console_enabled_ = configuration.consoleEnabled;
		file_.close();
		if (configuration.fileOutputPath) file_.open(*configuration.fileOutputPath, std::ios::app);
	}

	Level LoggerService::MinimumLevel()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return minimum_level_;
	}

	void LoggerService::SetConsoleEnabled(bool is_enabled)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		console_enabled_ = is_enabled;
	}

	bool LoggerService::SetFileOutput(const std::filesystem::path& path)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		file_.close();
		file_.open(path, std::ios::app);
		return file_.is_open();
	}

	void LoggerService::ClearFileOutput()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		file_.close();
	}

	void LoggerService::Write(Level level, std::string_view category, std::string_view message, SourceLocation source)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (!IsEnabled(level)) return;
		const Record record{ level, category, message, source, std::chrono::system_clock::now(), std::this_thread::get_id() };
		const std::string line = FormatRecord(record);
		if (console_enabled_) (level >= Level::Error ? std::cerr : std::cout) << line << '\n';
		if (file_.is_open()) file_ << line << '\n';
	}

	bool LoggerService::IsEnabled(Level level) const
	{
		return static_cast<int>(level) >= static_cast<int>(minimum_level_);
	}
}
