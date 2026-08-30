#pragma once

#include "voxel/WorldConfig.h"

namespace voxel::detail
{
	[[nodiscard]] bool IsWorldConfigSerializable(const WorldConfig& world) noexcept;
}
