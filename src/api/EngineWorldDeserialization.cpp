#include "voxel/Engine.h"

#include "WorldSerializationHelpers.h"

#include <cstdint>
#include <fstream>
#include <string>
#include <type_traits>
#include <utility>

namespace voxel
{
	namespace
	{
		bool TryLoadWorldCommand(
			WorldConfig& world,
			const detail::SerializedWorldLine& tokens)
		{
			auto result = detail::TryLoadWorldBasicCommand(world, tokens);
			if (result == detail::WorldCommandLoadResult::Unknown)
				result = detail::TryLoadWorldTerrainCommand(world, tokens);
			if (result == detail::WorldCommandLoadResult::Unknown)
				result = detail::TryLoadWorldEditCommand(world, tokens);
			return result == detail::WorldCommandLoadResult::Loaded;
		}

		bool TryLoadWorldStream(std::ifstream& file, WorldConfig& world)
		{
			bool has_header = false;
			std::uint8_t seen_singletons = 0;
			std::string line;
			detail::SerializedWorldLine tokens;
			while (std::getline(file, line))
			{
				if (!detail::TryTokenizeSerializedWorldLine(line, tokens)) return false;
				if (tokens.empty()) continue;
				if (!has_header)
				{
					if (!detail::IsSerializedWorldHeader(tokens)) return false;
					has_header = true;
					continue;
				}
				const std::uint8_t singleton_bit =
					detail::GetSerializedWorldSingletonBit(tokens[0]);
				if ((seen_singletons & singleton_bit) != 0) return false;
				if (!TryLoadWorldCommand(world, tokens)) return false;
				seen_singletons |= singleton_bit;
			}
			return has_header && file.eof() && !file.bad() &&
				detail::HasRequiredSerializedWorldSingletons(seen_singletons);
		}
	}

	bool TryLoadWorldConfig(const std::string& path, WorldConfig& destination)
	{
		static_assert(std::is_nothrow_move_assignable_v<WorldConfig>);
		try
		{
			std::ifstream file(path, std::ios::binary);
			if (!file) return false;
			WorldConfig parsed{};
			if (!TryLoadWorldStream(file, parsed)) return false;
			destination = std::move(parsed);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	WorldConfig LoadWorldConfig(const std::string& path)
	{
		WorldConfig loaded{};
		if (!TryLoadWorldConfig(path, loaded)) return {};
		return loaded;
	}
}
