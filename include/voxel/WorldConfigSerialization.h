#pragma once

#include "voxel/WorldConfiguration.h"

#include <string>

namespace voxel
{
	[[nodiscard]] bool SaveWorldConfig(const WorldConfig& world, const std::string& path);
	[[nodiscard]] WorldConfig LoadWorldConfig(const std::string& path);
}
