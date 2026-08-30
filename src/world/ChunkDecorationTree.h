#pragma once

#include "ChunkDecorationRules.h"

namespace ve::world::terrain::decorations::detail
{
	struct TreeDecorationRule
	{
		int x_divisor = 1;
		int x_remainder = 0;
		int z_divisor = 1;
		int z_remainder = 0;
		ve::blocks::BlockId log = ve::blocks::BlockId::OakLog;
		ve::blocks::BlockId leaves = ve::blocks::BlockId::OakLeaves;
	};

	bool TryAddTreePattern(
		int local_block_x,
		int local_block_z,
		int surface_y,
		float world_block_x,
		float world_block_z,
		const TreeDecorationRule& rule,
		BlockStorage& blocks);
}
