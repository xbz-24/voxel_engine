#include "WorldBlockEdit.h"

#include <algorithm>

namespace ve::world
{
	WorldBlockEdit MakeSingleBlockEdit(
		int target_block_x,
		int target_block_y,
		int target_block_z,
		ve::blocks::BlockId block_id) noexcept
	{
		return {
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
		return {
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
}
