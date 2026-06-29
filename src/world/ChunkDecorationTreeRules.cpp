#include "ChunkDecorationRules.h"

#include <array>
#include <cmath>

using ve::blocks::BlockId;

namespace ve::world::terrain::decorations::detail
{
	namespace
	{
		struct TreeDecorationRule
		{
			int x_divisor = 1;
			int x_remainder = 0;
			int z_divisor = 1;
			int z_remainder = 0;
			BlockId log = BlockId::OakLog;
			BlockId leaves = BlockId::OakLeaves;
		};

		bool ContainsLocalBlock(int local_block_x, int local_block_y, int local_block_z)
		{
			return local_block_x >= 0 && local_block_x < ChunkWidth &&
				local_block_y >= 0 && local_block_y < ChunkHeight &&
				local_block_z >= 0 && local_block_z < ChunkDepth;
		}

		bool WorldPatternMatches(float world_block_x, float world_block_z, const TreeDecorationRule& rule)
		{
			const int world_block_x_integer = static_cast<int>(world_block_x);
			const int world_block_z_integer = static_cast<int>(world_block_z);
			return world_block_x_integer % rule.x_divisor == rule.x_remainder &&
				world_block_z_integer % rule.z_divisor == rule.z_remainder;
		}

		void AddTree(int trunk_x, int trunk_z, int surface_y, BlockId log, BlockId leaves, BlockStorage& blocks)
		{
			for (int trunk_y = surface_y + 1; trunk_y <= surface_y + 4 && trunk_y < ChunkHeight; trunk_y++)
			{
				blocks[trunk_x][trunk_y][trunk_z] = log;
			}

			const int leaves_y = surface_y + 5;
			for (int leaf_x = trunk_x - 2; leaf_x <= trunk_x + 2 && leaves_y < ChunkHeight; leaf_x++)
			{
				for (int leaf_z = trunk_z - 2; leaf_z <= trunk_z + 2; leaf_z++)
				{
					if (ContainsLocalBlock(leaf_x, leaves_y, leaf_z) &&
						std::abs(leaf_x - trunk_x) + std::abs(leaf_z - trunk_z) <= 3)
					{
						blocks[leaf_x][leaves_y][leaf_z] = leaves;
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
			const bool tree_fits_inside_chunk = local_block_x > 1 && local_block_x < ChunkWidth - 2 &&
				local_block_z > 1 && local_block_z < ChunkDepth - 2;
			if (!tree_fits_inside_chunk || !WorldPatternMatches(world_block_x, world_block_z, rule)) return false;

			AddTree(local_block_x, local_block_z, surface_y, rule.log, rule.leaves, blocks);
			return true;
		}
	}

	bool TryAddTreeDecoration(
		int local_block_x,
		int local_block_z,
		int surface_y,
		float world_block_x,
		float world_block_z,
		BlockStorage& blocks)
	{
		constexpr std::array<TreeDecorationRule, 2> tree_rules{ {
			{ 29, 4, 31, 6, BlockId::OakLog, BlockId::OakLeaves },
			{ 43, 10, 47, 14, BlockId::BirchLog, BlockId::BirchLeaves },
		} };

		for (const TreeDecorationRule& rule : tree_rules)
		{
			if (TryAddTreePattern(local_block_x, local_block_z, surface_y, world_block_x, world_block_z, rule, blocks))
			{
				return true;
			}
		}
		return false;
	}
}
