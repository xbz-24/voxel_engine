#pragma once

#include "voxel/Blocks.h"
#include "voxel/WorldCoordinates.h"

namespace voxel
{
	/** @addtogroup voxel_sdk_world_editing
	 * @{
	 */
	/** Inclusive block-space box used by edit commands and scene helpers. */
	struct BlockBox
	{
		BlockPosition minimum{};
		BlockPosition maximum{};

		[[nodiscard]] static BlockBox Between(BlockPosition first, BlockPosition second) noexcept;
	};

	[[nodiscard]] BlockBox Box(BlockPosition first, BlockPosition second) noexcept;

	/**
	 * Public command payload for deterministic world edits. Fill boxes affect only
	 * their intersection with loaded chunks; coordinates outside that storage are
	 * ignored instead of determining iteration cost.
	 */
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
	/** @} */
}
