#include "ChunkDecorationTree.h"

#include <array>

namespace ve::world::terrain::decorations::detail
{
	bool TryAddTreeDecoration(
		int local_block_x,
		int local_block_z,
		int surface_y,
		float world_block_x,
		float world_block_z,
		BlockStorage& blocks)
	{
		constexpr std::array<TreeDecorationRule, 2> tree_rules{ {
			{ 29, 4, 31, 6, ve::blocks::BlockId::OakLog, ve::blocks::BlockId::OakLeaves },
			{ 43, 10, 47, 14, ve::blocks::BlockId::BirchLog, ve::blocks::BlockId::BirchLeaves },
		} };

		for (const TreeDecorationRule& rule : tree_rules)
		{
			if (TryAddTreePattern(
				local_block_x,
				local_block_z,
				surface_y,
				world_block_x,
				world_block_z,
				rule,
				blocks))
			{
				return true;
			}
		}
		return false;
	}
}
