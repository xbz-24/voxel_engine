#pragma once

#include "voxel/Blocks.h"
#include "voxel/WorldCoordinates.h"

namespace voxel
{
	/** Inclusive block-space box used by edit commands and scene helpers. */
	struct BlockBox
	{
		BlockPosition minimum{};
		BlockPosition maximum{};

		[[nodiscard]] static BlockBox Between(BlockPosition first, BlockPosition second) noexcept;
	};

	[[nodiscard]] BlockBox Box(BlockPosition first, BlockPosition second) noexcept;

	/** Public command payload for deterministic world edits. */
	struct WorldEdit
	{
		enum class Kind
		{
			SetBlock,
			FillBox
		};

		Kind kind = Kind::SetBlock;
		BlockPosition position{};
		BlockBox box{};
		Block block = Block::Air;
	};
}
