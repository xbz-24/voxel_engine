#pragma once

#include "voxel/WorldConfig.h"

#include <fstream>
#include <string>

namespace voxel::detail
{
	[[nodiscard]] int ToSerializedBlock(Block block) noexcept;
	[[nodiscard]] Block FromSerializedBlock(int value) noexcept;

	[[nodiscard]] const char* ToSerializedTerrainGenerator(TerrainGenerator generator) noexcept;
	[[nodiscard]] TerrainGenerator FromSerializedTerrainGenerator(const std::string& value) noexcept;
	[[nodiscard]] const char* ToSerializedTerrainBiome(TerrainBiome biome) noexcept;
	[[nodiscard]] TerrainBiome FromSerializedTerrainBiome(const std::string& value) noexcept;

	void SaveWorldTerrainConfig(std::ofstream& file, const WorldConfig& world);
	[[nodiscard]] bool TryLoadWorldTerrainCommand(
		WorldConfig& world,
		const std::string& command,
		std::ifstream& file);
}
