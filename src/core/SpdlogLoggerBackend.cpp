#include "SpdlogLoggerBackend.h"

#include "LogFormatter.h"

#include <spdlog/logger.h>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <vector>

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

		/** @param path Filesystem path. @return Filename type expected by spdlog. */
		spdlog::filename_t ToSpdlogFilename(const std::filesystem::path& path)
		{
#ifdef SPDLOG_WCHAR_FILENAMES
			return path.wstring();
#else
			return path.string();
#endif
		}
	}

	/** Creates a spdlog-backed logger with the default console sink. */
	SpdlogLoggerBackend::SpdlogLoggerBackend() { RebuildLogger(); }

	/** Applies minimum severity plus console and file sink settings. */
	void SpdlogLoggerBackend::ApplyConfiguration(const LoggerConfiguration& configuration)
	{
		minimum_level_ = configuration.minimumLevel;
		console_enabled_ = configuration.consoleEnabled;
		file_output_path_ = configuration.fileOutputPath;
		RebuildLogger();
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
	{ console_enabled_ = is_enabled; RebuildLogger(); }

	/** Opens a file sink and keeps the console setting unchanged. */
	bool SpdlogLoggerBackend::SetFileOutput(const std::filesystem::path& path)
	{ file_output_path_ = path; RebuildLogger(); return logger_ != nullptr; }

	/** Removes the file sink from the backend. */
	void SpdlogLoggerBackend::ClearFileOutput()
	{ file_output_path_.reset(); RebuildLogger(); }

	/** Writes one structured record through spdlog. */
	void SpdlogLoggerBackend::Write(const Record& record)
	{
		if (!logger_ || record.level < minimum_level_) return;
		const std::string message = FormatMessageWithFields(record);
		logger_->log(ToSpdlogLevel(record.level), "[{}] {} ({}:{} {})",
			record.category, message, record.source.file, record.source.line, record.source.function);
	}

	/** Rebuilds the sink list and recreates the internal logger. */
	void SpdlogLoggerBackend::RebuildLogger()
	{
		std::vector<spdlog::sink_ptr> sinks;
		if (console_enabled_) sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		if (file_output_path_) sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(ToSpdlogFilename(*file_output_path_), true));
		if (sinks.empty()) sinks.push_back(std::make_shared<spdlog::sinks::null_sink_mt>());
		logger_ = std::make_shared<spdlog::logger>("voxel_engine", sinks.begin(), sinks.end());
		logger_->set_pattern("%H:%M:%S [%^%l%$] [thread %t] %v");
		logger_->set_level(ToSpdlogLevel(minimum_level_));
		logger_->flush_on(spdlog::level::trace);
	}
}
