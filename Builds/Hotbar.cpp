#include "Hotbar.h"

namespace ve::gameplay
{
	const std::array<ve::blocks::BlockId, HotbarSlotCount>& DefaultHotbarBlocks()
	{
		static const std::array<ve::blocks::BlockId, HotbarSlotCount> blocks = {
			ve::blocks::BlockId::Grass,
			ve::blocks::BlockId::Dirt,
			ve::blocks::BlockId::Stone,
			ve::blocks::BlockId::Cobblestone,
			ve::blocks::BlockId::OakLog,
			ve::blocks::BlockId::OakPlanks,
			ve::blocks::BlockId::Sand,
			ve::blocks::BlockId::Bricks,
			ve::blocks::BlockId::DiamondOre
		};

		return blocks;
	}

	int HotbarIndexFor(ve::blocks::BlockId blockId)
	{
		const auto& blocks = DefaultHotbarBlocks();
		for (int index = 0; index < HotbarSlotCount; index++)
		{
			if (blocks[static_cast<std::size_t>(index)] == blockId)
			{
				return index;
			}
		}

		return 0;
	}
}

