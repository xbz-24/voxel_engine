#pragma once

#include <string>

namespace voxel
{
	/** Minimum severity routed through the engine logger. */
	enum class LogLevel
	{
		Default,
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};

	/** Logging policy translated into the runtime logger at startup. */
	struct LogSettings
	{
		LogLevel minimum_level = LogLevel::Default;
		bool console_enabled = true;
		bool file_output_enabled = false;
		std::string file_output_path;

		LogSettings& WithMinimumLevel(LogLevel level) noexcept;
		LogSettings& EnableConsole(bool enabled = true) noexcept;
		LogSettings& DisableConsole() noexcept;
		LogSettings& WriteToFile(std::string path);
		LogSettings& DisableFileOutput() noexcept;
	};
}
