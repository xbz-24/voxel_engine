#include "voxel/Engine.h"

#include "WorldSerializationHelpers.h"

namespace voxel::detail
{
	int ToSerializedBlock(Block block) noexcept
	{
		return static_cast<int>(block);
	}

	bool TryParseSerializedBlock(std::string_view value, Block& block) noexcept
	{
		int parsed = 0;
		if (!TryParseSerializedInt(value, parsed) ||
			parsed < ToSerializedBlock(Block::Air) ||
			parsed > ToSerializedBlock(Block::DeadBush))
		{
			return false;
		}
		block = static_cast<Block>(parsed);
		return true;
	}
}
