#include "WorldSerializationHelpers.h"

#include <charconv>
#include <sstream>
#include <system_error>
#include <utility>

namespace voxel::detail
{
	namespace
	{
		constexpr std::uint8_t SizeBit = 1U << 0;
		constexpr std::uint8_t TerrainSeedBit = 1U << 1;
		constexpr std::uint8_t SurfaceHeightBit = 1U << 2;
		constexpr std::uint8_t GeneratorBit = 1U << 3;
		constexpr std::uint8_t BiomeBit = 1U << 4;
		constexpr std::uint8_t PaletteBit = 1U << 5;
		constexpr std::uint8_t TerrainBits = 0x3e;
	}

	bool TryParseSerializedInt(std::string_view value, int& parsed) noexcept
	{
		const auto result = std::from_chars(value.data(), value.data() + value.size(), parsed);
		return result.ec == std::errc{} && result.ptr == value.data() + value.size();
	}

	bool TryParseSerializedUInt32(std::string_view value, std::uint32_t& parsed) noexcept
	{
		const auto result = std::from_chars(value.data(), value.data() + value.size(), parsed);
		return result.ec == std::errc{} && result.ptr == value.data() + value.size();
	}

	bool TryTokenizeSerializedWorldLine(
		const std::string& line,
		SerializedWorldLine& tokens)
	{
		tokens.clear();
		std::istringstream stream(line);
		std::string token;
		while (stream >> token) tokens.push_back(std::move(token));
		return stream.eof() && !stream.bad();
	}

	bool IsSerializedWorldHeader(const SerializedWorldLine& tokens) noexcept
	{
		return tokens.size() == 2 &&
			tokens[0] == "voxel-world-config" &&
			tokens[1] == "1";
	}

	std::uint8_t GetSerializedWorldSingletonBit(std::string_view command) noexcept
	{
		if (command == "size") return SizeBit;
		if (command == "terrain_seed") return TerrainSeedBit;
		if (command == "base_surface_height") return SurfaceHeightBit;
		if (command == "terrain_generator") return GeneratorBit;
		if (command == "terrain_biome") return BiomeBit;
		if (command == "terrain_palette") return PaletteBit;
		return 0;
	}

	bool HasRequiredSerializedWorldSingletons(std::uint8_t seen) noexcept
	{
		const auto terrain_seen = static_cast<std::uint8_t>(seen & TerrainBits);
		return (seen & SizeBit) != 0 &&
			(terrain_seen == 0 || terrain_seen == TerrainBits);
	}

	WorldCommandLoadResult TryLoadWorldBasicCommand(
		WorldConfig& world,
		const SerializedWorldLine& tokens) noexcept
	{
		if (tokens[0] != "size") return WorldCommandLoadResult::Unknown;

		int size_chunks = 0;
		if (tokens.size() != 2 || !TryParseSerializedInt(tokens[1], size_chunks))
		{
			return WorldCommandLoadResult::Invalid;
		}
		world.WithSizeChunks(size_chunks);
		return WorldCommandLoadResult::Loaded;
	}
}
