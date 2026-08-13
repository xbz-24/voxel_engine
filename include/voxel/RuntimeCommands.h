#pragma once

#include "voxel/WorldTypes.h"

#include <vector>

namespace voxel
{
	/** Mutating commands queued by a callback and applied after it returns. */
	struct RuntimeCommands
	{
		std::vector<WorldEdit> world_edits;
		bool request_close = false;

		RuntimeCommands& SetBlock(BlockPosition position, Block block);
		RuntimeCommands& SetBlock(int block_x, int block_y, int block_z, Block block);
		RuntimeCommands& FillBox(BlockBox box, Block block);
		RuntimeCommands& FillBox(BlockPosition first, BlockPosition second, Block block);
		RuntimeCommands& FillBox(int first_block_x, int first_block_y, int first_block_z,
			int second_block_x, int second_block_y, int second_block_z, Block block);
		RuntimeCommands& ClearBox(BlockPosition first, BlockPosition second);
		RuntimeCommands& ClearBox(int first_block_x, int first_block_y, int first_block_z,
			int second_block_x, int second_block_y, int second_block_z);
		RuntimeCommands& RequestClose() noexcept;
	};
}
