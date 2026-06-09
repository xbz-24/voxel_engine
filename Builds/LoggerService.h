#pragma once

#include "LoggerConfiguration.h"

#include <fstream>
#include <mutex>
#include <string_view>

namespace ve::log
{
	class LoggerService
	{
	public:
		/**
		 * Returns the process-wide logger object.
		 *
		 * @return Mutable logger singleton.
		 */
		static LoggerService& Instance();

		/** @param level Lowest severity that should be written. */
		void SetMinimumLevel(Level level);

		/** @param configuration Minimum level plus enabled sinks. */
		void ApplyConfiguration(const LoggerConfiguration& configuration);

		/** @return Lowest severity currently accepted by the logger. */
		Level MinimumLevel();

		/** @param is_enabled True to write records to stdout/stderr. */
		void SetConsoleEnabled(bool is_enabled);

		/** @param path File path receiving formatted records. @return True when opened. */
		bool SetFileOutput(const std::filesystem::path& path);

		/** Closes the file sink when one is active. */
		void ClearFileOutput();

		/** @param level Severity. @param category Subsystem. @param message Body. @param source Call site. */
		void Write(Level level, std::string_view category, std::string_view message, SourceLocation source);

	private:
		LoggerService() = default;
		bool IsEnabled(Level level) const;

		std::mutex mutex_;
		Level minimum_level_ = Level::Info;
		bool console_enabled_ = true;
		std::ofstream file_;
	};
}
