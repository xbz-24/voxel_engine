#include "WorldSerializationHelpers.h"

#include <array>

namespace voxel::detail
{
	namespace
	{
		bool TryParseEditValues(
			const SerializedWorldLine& tokens,
			std::array<int, 6>& coordinates,
			std::size_t coordinate_count,
			Block& block) noexcept
		{
			for (std::size_t index = 0; index < coordinate_count; ++index)
			{
				if (!TryParseSerializedInt(tokens[index + 1], coordinates[index])) return false;
			}
			return TryParseSerializedBlock(tokens.back(), block);
		}
	}

	WorldCommandLoadResult TryLoadWorldEditCommand(
		WorldConfig& world,
		const SerializedWorldLine& tokens)
	{
		if (tokens[0] == "set")
		{
			std::array<int, 6> coordinates{};
			Block block = Block::Air;
			if (tokens.size() != 5 ||
				!TryParseEditValues(tokens, coordinates, 3, block))
			{
				return WorldCommandLoadResult::Invalid;
			}
			world.SetBlock(coordinates[0], coordinates[1], coordinates[2], block);
			return WorldCommandLoadResult::Loaded;
		}
		if (tokens[0] != "fill") return WorldCommandLoadResult::Unknown;

		std::array<int, 6> coordinates{};
		Block block = Block::Air;
		if (tokens.size() != 8 || !TryParseEditValues(tokens, coordinates, 6, block))
		{
			return WorldCommandLoadResult::Invalid;
		}
		world.FillBox(
			coordinates[0], coordinates[1], coordinates[2],
			coordinates[3], coordinates[4], coordinates[5], block);
		return WorldCommandLoadResult::Loaded;
	}
}
