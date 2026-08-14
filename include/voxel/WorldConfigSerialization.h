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

	/**
	 * Strictly loads a versioned world configuration.
	 *
	 * Unknown commands or enum values, truncated records, trailing fields, and
	 * I/O failures return false without modifying destination. Version 1 requires
	 * exactly one size record and accepts either no terrain records or the complete
	 * five-record terrain group. Records may be reordered and blank lines ignored.
	 */
	[[nodiscard]] bool TryLoadWorldConfig(
		const std::string& path, WorldConfig& destination);

	/**
	 * Compatibility loader that returns a default world when strict loading fails.
	 * Use TryLoadWorldConfig when a default world must be distinguished from error.
	 */
	[[nodiscard]] WorldConfig LoadWorldConfig(const std::string& path);
}
