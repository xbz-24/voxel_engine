#include "voxel/Engine.h"

#include "CoreTypes.h"
#include "WorldSerializationHelpers.h"

namespace voxel::detail
{
	int ToSerializedBlock(Block block) noexcept
	{
		return ve::core::ToInt(ve::core::ToU8(block));
	}

	Block FromSerializedBlock(int value) noexcept
	{
		if (value < ToSerializedBlock(Block::Air) || value > ToSerializedBlock(Block::DeadBush))
		{
			return Block::Air;
		}
		return ve::core::NumericCast<Block>(ve::core::ToU8(value));
	}
}
