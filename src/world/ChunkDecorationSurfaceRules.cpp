#include "ChunkDecorationRules.h"

#include <array>

using ve::blocks::BlockId;

namespace ve::world::terrain::decorations::detail
{
	namespace
	{
		struct SurfaceDecorationRule
		{
			int maximum_surface_y = ChunkHeight;
			int pattern_divisor = 1;
			BlockId block = BlockId::Air;
		};

		bool ContainsLocalBlock(int local_block_x, int local_block_y, int local_block_z)
		{
			return local_block_x >= 0 && local_block_x < ChunkWidth &&
				local_block_y >= 0 && local_block_y < ChunkHeight &&
				local_block_z >= 0 && local_block_z < ChunkDepth;
		}

		void SetAboveSurface(int local_block_x, int local_block_z, int surface_y, BlockId block, BlockStorage& blocks)
		{
			const int local_block_y = surface_y + 1;
			if (ContainsLocalBlock(local_block_x, local_block_y, local_block_z))
			{
				blocks[local_block_x][local_block_y][local_block_z] = block;
			}
		}

		bool TrySetDecorationBlock(
			int local_block_x,
			int local_block_z,
			int surface_y,
			int surface_block_pattern,
			const SurfaceDecorationRule& rule,
			BlockStorage& blocks)
		{
			if (surface_y >= rule.maximum_surface_y || surface_block_pattern % rule.pattern_divisor != 0) return false;
			SetAboveSurface(local_block_x, local_block_z, surface_y, rule.block, blocks);
			return true;
		}
	}

	bool TryAddSurfaceDecoration(
		int local_block_x,
		int local_block_z,
		int surface_y,
		int surface_block_pattern,
		BlockStorage& blocks)
	{
		constexpr std::array<SurfaceDecorationRule, 5> decoration_rules{ {
			{ 61, 43, BlockId::Pumpkin },
			{ 57, 47, BlockId::Melon },
			{ ChunkHeight, 53, BlockId::HayBlock },
			{ ChunkHeight, 59, BlockId::CraftingTable },
			{ ChunkHeight, 61, BlockId::Bookshelf },
		} };

		for (const SurfaceDecorationRule& rule : decoration_rules)
		{
			if (TrySetDecorationBlock(local_block_x, local_block_z, surface_y, surface_block_pattern, rule, blocks))
			{
				return true;
			}
		}
		return false;
	}
}
