#pragma once

#include "voxel/GraphicsConfig.h"
#include "voxel/Logging.h"
#include "voxel/TerrainConfig.h"

namespace voxel::detail::config_validation
{
	[[nodiscard]] bool IsKnownPublicGraphicsApi(GraphicsApi api) noexcept;
	[[nodiscard]] bool IsKnownPublicLogLevel(LogLevel level) noexcept;
	[[nodiscard]] bool IsKnownPublicTerrainGenerator(TerrainGenerator generator) noexcept;
	[[nodiscard]] bool IsKnownPublicTerrainBiome(TerrainBiome biome) noexcept;
	[[nodiscard]] bool IsKnownPublicTerrainPalette(const TerrainPalette& palette) noexcept;
}
