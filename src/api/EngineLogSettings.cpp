#include "voxel/Engine.h"

#include <string>
#include <utility>

namespace voxel
{
	LogSettings& LogSettings::WithMinimumLevel(LogLevel level) noexcept
	{
		minimum_level = level;
		return *this;
	}

	LogSettings& LogSettings::EnableConsole(bool enabled) noexcept
	{
		console_enabled = enabled;
		return *this;
	}

	LogSettings& LogSettings::DisableConsole() noexcept
	{
		return EnableConsole(false);
	}

	LogSettings& LogSettings::WriteToFile(std::string path)
	{
		file_output_enabled = true;
		file_output_path = std::move(path);
		return *this;
	}

	LogSettings& LogSettings::DisableFileOutput() noexcept
	{
		file_output_enabled = false;
		file_output_path.clear();
		return *this;
	}
}
