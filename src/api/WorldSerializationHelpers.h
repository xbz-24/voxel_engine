#pragma once

#include "voxel/WorldConfig.h"

#include <cstdint>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

namespace voxel::detail
{
	using SerializedWorldLine = std::vector<std::string>;

	enum class WorldCommandLoadResult
	{
		Unknown,
		Loaded,
		Invalid
	};

	[[nodiscard]] int ToSerializedBlock(Block block) noexcept;
	[[nodiscard]] bool TryParseSerializedBlock(std::string_view value, Block& block) noexcept;
	[[nodiscard]] bool TryParseSerializedInt(std::string_view value, int& parsed) noexcept;
	[[nodiscard]] bool TryParseSerializedUInt32(
		std::string_view value,
		std::uint32_t& parsed) noexcept;

	[[nodiscard]] const char* ToSerializedTerrainGenerator(TerrainGenerator generator) noexcept;
	[[nodiscard]] bool TryParseSerializedTerrainGenerator(
		std::string_view value,
		TerrainGenerator& generator) noexcept;
	[[nodiscard]] const char* ToSerializedTerrainBiome(TerrainBiome biome) noexcept;
	[[nodiscard]] bool TryParseSerializedTerrainBiome(
		std::string_view value,
		TerrainBiome& biome) noexcept;

	void SaveWorldTerrainConfig(std::ofstream& file, const WorldConfig& world);
	[[nodiscard]] bool TryTokenizeSerializedWorldLine(
		const std::string& line,
		SerializedWorldLine& tokens);
	[[nodiscard]] bool IsSerializedWorldHeader(const SerializedWorldLine& tokens) noexcept;
	[[nodiscard]] std::uint8_t GetSerializedWorldSingletonBit(std::string_view command) noexcept;
	[[nodiscard]] bool HasRequiredSerializedWorldSingletons(std::uint8_t seen) noexcept;
	[[nodiscard]] WorldCommandLoadResult TryLoadWorldBasicCommand(
		WorldConfig& world,
		const SerializedWorldLine& tokens) noexcept;
	[[nodiscard]] WorldCommandLoadResult TryLoadWorldTerrainCommand(
		WorldConfig& world,
		const SerializedWorldLine& tokens) noexcept;
	[[nodiscard]] WorldCommandLoadResult TryLoadWorldEditCommand(
		WorldConfig& world,
		const SerializedWorldLine& tokens);
}
