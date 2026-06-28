#pragma once

#include "LoggerConfiguration.h"
#include "SpdlogLoggerBackend.h"

#include <functional>
#include <mutex>
#include <string>
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

		/** @param callback Optional sink receiving formatted log lines after built-in sinks. */
		void SetCallback(std::function<void(std::string)> callback);

		/** Closes the file sink when one is active. */
		void ClearFileOutput();

		/** @param level Severity. @param category Subsystem. @param message Body. @param source Call site. */
		void Write(Level level, std::string_view category, std::string_view message, SourceLocation source);

	private:
		LoggerService() = default;

		// TODO: Add structured fields and per-sink formatting once diagnostics need machine-readable logs.
		std::mutex mutex_;
		SpdlogLoggerBackend backend_;
		std::function<void(std::string)> callback_;
	};
}
