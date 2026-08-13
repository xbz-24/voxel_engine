#include "voxel/Engine.h"

#include "WorldSerializationHelpers.h"

namespace voxel::detail
{
	int ToSerializedBlock(Block block) noexcept
	{
		return static_cast<int>(block);
	}

	Block FromSerializedBlock(int value) noexcept
	{
		if (value < ToSerializedBlock(Block::Air) || value > ToSerializedBlock(Block::DeadBush))
		{
			return Block::Air;
		}
		return static_cast<Block>(value);
	}
}
