#include "WorldBlockEdit.h"

#include "World.h"
#include "WorldBlockEditRange.h"

#include <cstdint>
#include <limits>

namespace ve::world
{
	namespace
	{
		[[nodiscard]] bool AddCoveredBlocks(
			std::uint64_t& covered_block_count,
			std::uint64_t additional_block_count) noexcept
		{
			if (additional_block_count >
				std::numeric_limits<std::uint64_t>::max() - covered_block_count)
				return false;
			covered_block_count += additional_block_count;
			return true;
		}

		[[nodiscard]] bool ApplyBlockBoxEdit(World& world, const WorldBlockEdit& world_block_edit)
		{
			bool every_block_write_was_accepted = true;
			const InclusiveBlockEditRange block_edit_range = NormalizeBlockEditRange(world_block_edit);
			const auto requested_block_count = TryCountBlocks(block_edit_range);
			std::uint64_t covered_block_count = 0;
			bool covered_block_count_is_valid = true;
			for (const Chunk& chunk : world.Chunks())
			{
				const auto intersection = IntersectBlockEditRange(block_edit_range, chunk);
				if (!intersection) continue;
				const auto intersection_block_count = TryCountBlocks(*intersection);
				if (covered_block_count_is_valid &&
					(!intersection_block_count ||
						!AddCoveredBlocks(covered_block_count, *intersection_block_count)))
					covered_block_count_is_valid = false;
				for (std::int64_t edit_block_z = intersection->minimum_block_z;
					edit_block_z <= intersection->maximum_block_z; ++edit_block_z)
				{
					for (std::int64_t edit_block_y = intersection->minimum_block_y;
						edit_block_y <= intersection->maximum_block_y; ++edit_block_y)
					{
						for (std::int64_t edit_block_x = intersection->minimum_block_x;
							edit_block_x <= intersection->maximum_block_x; ++edit_block_x)
						{
							every_block_write_was_accepted = world.SetBlock(
								static_cast<int>(edit_block_x), static_cast<int>(edit_block_y),
								static_cast<int>(edit_block_z), world_block_edit.block) &&
								every_block_write_was_accepted;
						}
					}
				}
			}
			return requested_block_count.has_value() && covered_block_count_is_valid &&
				covered_block_count == *requested_block_count && every_block_write_was_accepted;
		}
	}

	bool ApplyWorldBlockEdit(World& world, const WorldBlockEdit& world_block_edit)
	{
		switch (world_block_edit.kind)
		{
		case WorldBlockEdit::Kind::SetBlock:
			return world.SetBlock(
				world_block_edit.minimum_block_x,
				world_block_edit.minimum_block_y,
				world_block_edit.minimum_block_z,
				world_block_edit.block);
		case WorldBlockEdit::Kind::FillBox:
			return ApplyBlockBoxEdit(world, world_block_edit);
		default:
			return false;
		}
	}
}
