#pragma once

#include <cstdint>
#include <optional>

class Chunk;

namespace ve::world
{
	struct WorldBlockEdit;

	struct InclusiveBlockEditRange
	{
		std::int64_t minimum_block_x = 0;
		std::int64_t minimum_block_y = 0;
		std::int64_t minimum_block_z = 0;
		std::int64_t maximum_block_x = 0;
		std::int64_t maximum_block_y = 0;
		std::int64_t maximum_block_z = 0;
	};

	[[nodiscard]] InclusiveBlockEditRange NormalizeBlockEditRange(
		const WorldBlockEdit& edit) noexcept;
	[[nodiscard]] std::optional<InclusiveBlockEditRange> IntersectBlockEditRange(
		const InclusiveBlockEditRange& range, const Chunk& chunk) noexcept;
	[[nodiscard]] std::optional<std::uint64_t> TryCountBlocks(
		const InclusiveBlockEditRange& range) noexcept;
}
