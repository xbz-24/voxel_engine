#include "voxel/Engine.h"

namespace voxel
{
	RuntimeCommands& RuntimeCommands::SetBlock(BlockPosition position, Block block)
	{
		world_edits.push_back(WorldEdit{
			WorldEdit::Kind::SetBlock,
			position,
			{},
			block
		});
		return *this;
	}

	RuntimeCommands& RuntimeCommands::SetBlock(int block_x, int block_y, int block_z, Block block)
	{
		return SetBlock(At(block_x, block_y, block_z), block);
	}

	RuntimeCommands& RuntimeCommands::FillBox(BlockBox box, Block block)
	{
		world_edits.push_back(WorldEdit{
			WorldEdit::Kind::FillBox,
			{},
			box,
			block
		});
		return *this;
	}

	RuntimeCommands& RuntimeCommands::FillBox(BlockPosition first, BlockPosition second, Block block)
	{
		return FillBox(Box(first, second), block);
	}

	RuntimeCommands& RuntimeCommands::FillBox(
		int first_block_x,
		int first_block_y,
		int first_block_z,
		int second_block_x,
		int second_block_y,
		int second_block_z,
		Block block)
	{
		return FillBox(
			Box(
				At(first_block_x, first_block_y, first_block_z),
				At(second_block_x, second_block_y, second_block_z)),
			block);
	}

	RuntimeCommands& RuntimeCommands::ClearBox(BlockPosition first, BlockPosition second)
	{
		return FillBox(first, second, Air);
	}

	RuntimeCommands& RuntimeCommands::ClearBox(
		int first_block_x,
		int first_block_y,
		int first_block_z,
		int second_block_x,
		int second_block_y,
		int second_block_z)
	{
		return FillBox(
			first_block_x,
			first_block_y,
			first_block_z,
			second_block_x,
			second_block_y,
			second_block_z,
			Air);
	}

	RuntimeCommands& RuntimeCommands::RequestClose() noexcept
	{
		request_close = true;
		return *this;
	}
}
