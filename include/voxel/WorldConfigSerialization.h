#pragma once

#include "voxel/WorldConfiguration.h"

#include <string>

namespace voxel
{
	/**
	 * Saves a world configuration. Unknown public enum values are rejected before
	 * the destination path is opened; I/O failures are also reported as false.
	 */
	[[nodiscard]] bool SaveWorldConfig(const WorldConfig& world, const std::string& path);
	[[nodiscard]] WorldConfig LoadWorldConfig(const std::string& path);
}
