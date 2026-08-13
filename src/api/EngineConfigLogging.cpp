#include "voxel/Engine.h"

#include <string>
#include <utility>

namespace voxel
{
	EngineConfig& EngineConfig::WithLogging(LogSettings value) noexcept
	{
		logging = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::WithLogLevel(LogLevel value) noexcept
	{
		logging.minimum_level = value;
		return *this;
	}

	EngineConfig& EngineConfig::LogToFile(std::string path)
	{
		logging.WriteToFile(std::move(path));
		return *this;
	}

	EngineConfig& EngineConfig::DisableFileLogging() noexcept
	{
		logging.DisableFileOutput();
		return *this;
	}

	EngineConfig& EngineConfig::EnableConsoleLogging(bool enabled) noexcept
	{
		logging.EnableConsole(enabled);
		return *this;
	}
}
