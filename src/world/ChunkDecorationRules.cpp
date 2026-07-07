#include "ChunkDecorationRules.h"

using ve::blocks::BlockId;

namespace ve::world::terrain::decorations::detail
{
	namespace
	{
		bool WorldPatternMatches(
			float world_block_x,
			float world_block_z,
			int x_divisor,
			int x_remainder,
			int z_divisor,
			int z_remainder)
		{
			const int world_block_x_integer = static_cast<int>(world_block_x);
			const int world_block_z_integer = static_cast<int>(world_block_z);
			return world_block_x_integer % x_divisor == x_remainder &&
				world_block_z_integer % z_divisor == z_remainder;
		}
	}

	void AddStoneDetails(
		int local_block_x,
		int local_block_z,
		int surface_y,
		float world_block_x,
		float world_block_z,
		BlockStorage& blocks)
	{
		if (WorldPatternMatches(world_block_x, world_block_z, 23, 0, 17, 0))
		{
			for (int local_block_y = surface_y + 1;
				local_block_y <= surface_y + 3 && local_block_y < ChunkHeight;
				local_block_y++)
			{
				blocks[local_block_x][local_block_y][local_block_z] = BlockId::Cobblestone;
			}
		}
		if (WorldPatternMatches(world_block_x, world_block_z, 41, 8, 37, 12))
		{
			for (int local_block_y = surface_y + 1;
				local_block_y <= surface_y + 2 && local_block_y < ChunkHeight;
				local_block_y++)
			{
				blocks[local_block_x][local_block_y][local_block_z] = BlockId::MossyCobblestone;
			}
		}
	}
}
