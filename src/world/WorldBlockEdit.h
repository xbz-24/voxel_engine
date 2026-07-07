#pragma once

#include "Block.h"

namespace ve::world
{
	class World;

	struct WorldBlockEdit
	{
		enum class Kind
		{
			SetBlock,
			FillBox
		};

		Kind kind = Kind::SetBlock;
		int minimum_block_x = 0;
		int minimum_block_y = 0;
		int minimum_block_z = 0;
		int maximum_block_x = 0;
		int maximum_block_y = 0;
		int maximum_block_z = 0;
		ve::blocks::BlockId block = ve::blocks::BlockId::Air;
	};

	[[nodiscard]] WorldBlockEdit MakeSingleBlockEdit(
		int target_block_x,
		int target_block_y,
		int target_block_z,
		ve::blocks::BlockId block_id) noexcept;

	[[nodiscard]] WorldBlockEdit MakeBlockBoxEdit(
		int first_corner_block_x,
		int first_corner_block_y,
		int first_corner_block_z,
		int second_corner_block_x,
		int second_corner_block_y,
		int second_corner_block_z,
		ve::blocks::BlockId block_id) noexcept;

	[[nodiscard]] bool ApplyWorldBlockEdit(World& world, const WorldBlockEdit& world_block_edit);
}
