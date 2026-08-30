#include "SpdlogLoggerBackend.h"

#include "LogFormatter.h"

#include <spdlog/logger.h>
#include <spdlog/common.h>

namespace ve::log
{
	namespace
	{
		/** @param level Engine severity. @return Equivalent spdlog severity. */
		spdlog::level::level_enum ToSpdlogLevel(Level level) noexcept
		{
			switch (level)
			{
			case Level::Trace: return spdlog::level::trace;
			case Level::Debug: return spdlog::level::debug;
			case Level::Info: return spdlog::level::info;
			case Level::Warning: return spdlog::level::warn;
			case Level::Error: return spdlog::level::err;
			case Level::Fatal: return spdlog::level::critical;
			}
			return spdlog::level::info;
		}
	}

	/** Creates a spdlog-backed logger with the default console sink. */
	SpdlogLoggerBackend::SpdlogLoggerBackend() { (void)RebuildLogger(); }

	/** Applies minimum severity plus console and file sink settings. */
	bool SpdlogLoggerBackend::ApplyConfiguration(const LoggerConfiguration& configuration)
	{
		minimum_level_ = configuration.minimumLevel;
		console_enabled_ = configuration.consoleEnabled;
		file_output_path_ = configuration.fileOutputPath;
		return RebuildLogger();
	}

	/** Sets the lowest accepted severity without rebuilding sinks. */
	void SpdlogLoggerBackend::SetMinimumLevel(Level level)
	{
		minimum_level_ = level;
		if (logger_) logger_->set_level(ToSpdlogLevel(level));
	}

	/** Returns the lowest accepted severity. */
	Level SpdlogLoggerBackend::MinimumLevel() const noexcept { return minimum_level_; }

	/** Enables or disables the console sink. */
	void SpdlogLoggerBackend::SetConsoleEnabled(bool is_enabled)
	{ console_enabled_ = is_enabled; (void)RebuildLogger(); }

	/** Opens a file sink and keeps the console setting unchanged. */
	bool SpdlogLoggerBackend::SetFileOutput(const std::filesystem::path& path)
	{ file_output_path_ = path; return RebuildLogger(); }

	/** Removes the file sink from the backend. */
	void SpdlogLoggerBackend::ClearFileOutput()
	{ file_output_path_.reset(); (void)RebuildLogger(); }

	/** Releases runtime sinks before rebuilding the process-default console logger. */
	void SpdlogLoggerBackend::ResetRuntimeState() noexcept
	{
		minimum_level_ = Level::Info;
		console_enabled_ = true;
		file_output_path_.reset();
		logger_.reset();
		try { (void)RebuildLogger(); }
		catch (...) { logger_.reset(); }
	}

	/** Writes one structured record through spdlog. */
	void SpdlogLoggerBackend::Write(const Record& record)
	{
		if (!logger_ || record.level < minimum_level_) return;
		const std::string message = FormatMessageWithFields(record);
		logger_->log(ToSpdlogLevel(record.level), "[{}] {} ({}:{} {})",
			record.category, message, record.source.file, record.source.line, record.source.function);
	}
}
