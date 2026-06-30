#include "WorldBlockEdit.h"

#include "World.h"

#include <algorithm>

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

			for (int edit_block_z = block_edit_range.minimum_block_z;
				edit_block_z <= block_edit_range.maximum_block_z;
				++edit_block_z)
			{
				for (int edit_block_y = block_edit_range.minimum_block_y;
					edit_block_y <= block_edit_range.maximum_block_y;
					++edit_block_y)
				{
					for (int edit_block_x = block_edit_range.minimum_block_x;
						edit_block_x <= block_edit_range.maximum_block_x;
						++edit_block_x)
					{
						every_block_write_was_accepted =
							world.SetBlock(edit_block_x, edit_block_y, edit_block_z, world_block_edit.block) &&
							every_block_write_was_accepted;
					}
				}
			}

			return every_block_write_was_accepted;
		}
	}

	WorldBlockEdit MakeSingleBlockEdit(
		int target_block_x,
		int target_block_y,
		int target_block_z,
		ve::blocks::BlockId block_id) noexcept
	{
		return WorldBlockEdit{
			WorldBlockEdit::Kind::SetBlock,
			target_block_x,
			target_block_y,
			target_block_z,
			target_block_x,
			target_block_y,
			target_block_z,
			block_id
		};
	}

	WorldBlockEdit MakeBlockBoxEdit(
		int first_corner_block_x,
		int first_corner_block_y,
		int first_corner_block_z,
		int second_corner_block_x,
		int second_corner_block_y,
		int second_corner_block_z,
		ve::blocks::BlockId block_id) noexcept
	{
		return WorldBlockEdit{
			WorldBlockEdit::Kind::FillBox,
			std::min(first_corner_block_x, second_corner_block_x),
			std::min(first_corner_block_y, second_corner_block_y),
			std::min(first_corner_block_z, second_corner_block_z),
			std::max(first_corner_block_x, second_corner_block_x),
			std::max(first_corner_block_y, second_corner_block_y),
			std::max(first_corner_block_z, second_corner_block_z),
			block_id
		};
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
