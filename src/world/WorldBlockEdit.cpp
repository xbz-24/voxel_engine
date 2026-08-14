#include "WorldBlockEdit.h"

#include "World.h"

#include <algorithm>
#include <cstdint>

namespace ve::world
{
	namespace
	{
		struct InclusiveBlockEditRange
		{
			int minimum_block_x = 0;
			int minimum_block_y = 0;
			int minimum_block_z = 0;
			int maximum_block_x = 0;
			int maximum_block_y = 0;
			int maximum_block_z = 0;
		};

		[[nodiscard]] InclusiveBlockEditRange NormalizeBlockEditRange(
			const WorldBlockEdit& world_block_edit) noexcept
		{
			return InclusiveBlockEditRange{
				std::min(world_block_edit.minimum_block_x, world_block_edit.maximum_block_x),
				std::min(world_block_edit.minimum_block_y, world_block_edit.maximum_block_y),
				std::min(world_block_edit.minimum_block_z, world_block_edit.maximum_block_z),
				std::max(world_block_edit.minimum_block_x, world_block_edit.maximum_block_x),
				std::max(world_block_edit.minimum_block_y, world_block_edit.maximum_block_y),
				std::max(world_block_edit.minimum_block_z, world_block_edit.maximum_block_z)
			};
		}

		[[nodiscard]] bool ApplyBlockBoxEdit(World& world, const WorldBlockEdit& world_block_edit)
		{
			bool every_block_write_was_accepted = true;
			const InclusiveBlockEditRange block_edit_range = NormalizeBlockEditRange(world_block_edit);

			for (std::int64_t edit_block_z = block_edit_range.minimum_block_z;
				edit_block_z <= block_edit_range.maximum_block_z;
				++edit_block_z)
			{
				for (std::int64_t edit_block_y = block_edit_range.minimum_block_y;
					edit_block_y <= block_edit_range.maximum_block_y;
					++edit_block_y)
				{
					for (std::int64_t edit_block_x = block_edit_range.minimum_block_x;
						edit_block_x <= block_edit_range.maximum_block_x;
						++edit_block_x)
					{
						every_block_write_was_accepted =
							world.SetBlock(
								static_cast<int>(edit_block_x),
								static_cast<int>(edit_block_y),
								static_cast<int>(edit_block_z),
								world_block_edit.block) &&
							every_block_write_was_accepted;
					}
				}
			}

			return every_block_write_was_accepted;
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
