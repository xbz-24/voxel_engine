#pragma once

#include "LoggerConfiguration.h"
#include "LogRecord.h"

#include <filesystem>
#include <memory>
#include <optional>

namespace spdlog
{
	class logger;
}

namespace ve::log
{
	/** Owns the third-party spdlog logger while keeping engine-facing logging stable. */
	class SpdlogLoggerBackend
	{
	public:
		/** Creates the backend with a console sink enabled by default. */
		SpdlogLoggerBackend();

		/** @param configuration Minimum level and sink configuration to apply. */
		void ApplyConfiguration(const LoggerConfiguration& configuration);

		/** @param level Lowest severity accepted by the backend. */
		void SetMinimumLevel(Level level);

		/** @return Lowest severity currently accepted by the backend. */
		[[nodiscard]] Level MinimumLevel() const noexcept;

		/** @param is_enabled True when console output should be active. */
		void SetConsoleEnabled(bool is_enabled);

		/** @param path Append-only file path for log output. @return True when opened. */
		[[nodiscard]] bool SetFileOutput(const std::filesystem::path& path);

		/** Removes the active file sink and keeps the other sinks alive. */
		void ClearFileOutput();

		/** @param record Structured log record to write through spdlog. */
		void Write(const Record& record);

	private:
		/** Recreates the spdlog logger after a sink setting changed. */
		void RebuildLogger();

		Level minimum_level_ = Level::Info;
		bool console_enabled_ = true;
		std::optional<std::filesystem::path> file_output_path_;
		std::shared_ptr<spdlog::logger> logger_;
	};
}
