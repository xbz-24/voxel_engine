#include "WorldBlockEditRange.h"

#include "Chunk.h"
#include "WorldBlockEdit.h"

#include <algorithm>
#include <limits>

namespace ve::world
{
	namespace
	{
		[[nodiscard]] std::optional<std::uint64_t> TryMultiply(
			std::uint64_t left, std::uint64_t right) noexcept
		{
			if (left != 0 && right > std::numeric_limits<std::uint64_t>::max() / left)
				return std::nullopt;
			return left * right;
		}

		[[nodiscard]] std::uint64_t InclusiveSpan(
			std::int64_t minimum, std::int64_t maximum) noexcept
		{
			return static_cast<std::uint64_t>(maximum - minimum) + 1U;
		}
	}

	InclusiveBlockEditRange NormalizeBlockEditRange(const WorldBlockEdit& edit) noexcept
	{
		return {
			std::min(edit.minimum_block_x, edit.maximum_block_x),
			std::min(edit.minimum_block_y, edit.maximum_block_y),
			std::min(edit.minimum_block_z, edit.maximum_block_z),
			std::max(edit.minimum_block_x, edit.maximum_block_x),
			std::max(edit.minimum_block_y, edit.maximum_block_y),
			std::max(edit.minimum_block_z, edit.maximum_block_z)
		};
	}

	std::optional<InclusiveBlockEditRange> IntersectBlockEditRange(
		const InclusiveBlockEditRange& range, const Chunk& chunk) noexcept
	{
		const std::int64_t chunk_minimum_x =
			static_cast<std::int64_t>(chunk.GetChunkX()) * Chunk::CHUNK_WIDTH;
		const std::int64_t chunk_minimum_z =
			static_cast<std::int64_t>(chunk.GetChunkZ()) * Chunk::CHUNK_DEPTH;
		InclusiveBlockEditRange intersection{
			std::max(range.minimum_block_x, chunk_minimum_x),
			std::max<std::int64_t>(range.minimum_block_y, 0),
			std::max(range.minimum_block_z, chunk_minimum_z),
			std::min(range.maximum_block_x, chunk_minimum_x + Chunk::CHUNK_WIDTH - 1),
			std::min<std::int64_t>(range.maximum_block_y, Chunk::CHUNK_HEIGHT - 1),
			std::min(range.maximum_block_z, chunk_minimum_z + Chunk::CHUNK_DEPTH - 1)
		};
		if (intersection.minimum_block_x > intersection.maximum_block_x ||
			intersection.minimum_block_y > intersection.maximum_block_y ||
			intersection.minimum_block_z > intersection.maximum_block_z)
			return std::nullopt;
		return intersection;
	}

	std::optional<std::uint64_t> TryCountBlocks(
		const InclusiveBlockEditRange& range) noexcept
	{
		const auto xy = TryMultiply(
			InclusiveSpan(range.minimum_block_x, range.maximum_block_x),
			InclusiveSpan(range.minimum_block_y, range.maximum_block_y));
		if (!xy) return std::nullopt;
		return TryMultiply(*xy,
			InclusiveSpan(range.minimum_block_z, range.maximum_block_z));
	}
}
