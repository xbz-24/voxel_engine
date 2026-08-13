#include "ChunkDecorationTree.h"

#include "ChunkTerrainMath.h"

#include <cmath>

namespace ve::world::terrain::decorations::detail
{
	namespace
	{
		bool ContainsLocalBlock(int x, int y, int z)
		{
			return x >= 0 && x < ChunkWidth &&
				y >= 0 && y < ChunkHeight &&
				z >= 0 && z < ChunkDepth;
		}

		bool WorldPatternMatches(float world_x, float world_z, const TreeDecorationRule& rule)
		{
			const int x = ve::world::terrain::detail::WorldBlockPatternCoordinate(world_x);
			const int z = ve::world::terrain::detail::WorldBlockPatternCoordinate(world_z);
			return x % rule.x_divisor == rule.x_remainder &&
				z % rule.z_divisor == rule.z_remainder;
		}

		void AddTree(
			int trunk_x,
			int trunk_z,
			int surface_y,
			ve::blocks::BlockId log,
			ve::blocks::BlockId leaves,
			BlockStorage& blocks)
		{
			for (int y = surface_y + 1; y <= surface_y + 4 && y < ChunkHeight; ++y)
			{
				blocks[trunk_x][y][trunk_z] = log;
			}
			const int leaves_y = surface_y + 5;
			for (int x = trunk_x - 2; x <= trunk_x + 2 && leaves_y < ChunkHeight; ++x)
			{
				for (int z = trunk_z - 2; z <= trunk_z + 2; ++z)
				{
					if (ContainsLocalBlock(x, leaves_y, z) &&
						std::abs(x - trunk_x) + std::abs(z - trunk_z) <= 3)
					{
						blocks[x][leaves_y][z] = leaves;
					}
				}
			}
		}
	}

	bool TryAddTreePattern(
		int local_block_x,
		int local_block_z,
		int surface_y,
		float world_block_x,
		float world_block_z,
		const TreeDecorationRule& rule,
		BlockStorage& blocks)
	{
		const bool fits = local_block_x > 1 && local_block_x < ChunkWidth - 2 &&
			local_block_z > 1 && local_block_z < ChunkDepth - 2;
		if (!fits || !WorldPatternMatches(world_block_x, world_block_z, rule)) return false;
		AddTree(local_block_x, local_block_z, surface_y, rule.log, rule.leaves, blocks);
		return true;
	}
}
