#include "ChunkDecorations.h"

#include <cmath>

using ve::blocks::BlockId;

namespace ve::world::terrain::decorations
{
	namespace
	{
		/**
		 * Checks if a local block coordinate is writable.
		 */
		bool ContainsLocalBlock(int x, int y, int z)
		{
			return x >= 0 && x < ChunkWidth && y >= 0 && y < ChunkHeight && z >= 0 && z < ChunkDepth;
		}

		/**
		 * Adds a small tree whose leaves stay inside this chunk.
		 */
		void AddTree(int x, int z, int surfaceHeight, BlockId log, BlockId leaves, BlockStorage& blocks)
		{
			for (int y = surfaceHeight + 1; y <= surfaceHeight + 4 && y < ChunkHeight; y++)
			{
				blocks[x][y][z] = log;
			}
			const int leavesY = surfaceHeight + 5;
			for (int lx = x - 2; lx <= x + 2 && leavesY < ChunkHeight; lx++)
			{
				for (int lz = z - 2; lz <= z + 2; lz++)
				{
					if (ContainsLocalBlock(lx, leavesY, lz) && std::abs(lx - x) + std::abs(lz - z) <= 3)
					{
						blocks[lx][leavesY][lz] = leaves;
					}
				}
			}
		}

		/**
		 * Places a block above the surface when the coordinate is valid.
		 */
		void SetAboveSurface(int x, int z, int surfaceHeight, BlockId blockId, BlockStorage& blocks)
		{
			const int y = surfaceHeight + 1;
			if (ContainsLocalBlock(x, y, z))
			{
				blocks[x][y][z] = blockId;
			}
		}

		/**
		 * Adds small generated ruin columns.
		 */
		void AddStoneDetails(int x, int z, int surfaceHeight, float globalX, float globalZ, BlockStorage& blocks)
		{
			if ((static_cast<int>(globalX) % 23 == 0) && (static_cast<int>(globalZ) % 17 == 0))
			{
				for (int y = surfaceHeight + 1; y <= surfaceHeight + 3 && y < ChunkHeight; y++) blocks[x][y][z] = BlockId::Cobblestone;
			}
			if ((static_cast<int>(globalX) % 41 == 8) && (static_cast<int>(globalZ) % 37 == 12))
			{
				for (int y = surfaceHeight + 1; y <= surfaceHeight + 2 && y < ChunkHeight; y++) blocks[x][y][z] = BlockId::MossyCobblestone;
			}
		}
	}

	void ApplySurfaceDecorations(int x, int z, int surfaceHeight, float globalX, float globalZ, int surfacePattern, BlockStorage& blocks)
	{
		AddStoneDetails(x, z, surfaceHeight, globalX, globalZ, blocks);
		const bool canFitTree = x > 1 && x < ChunkWidth - 2 && z > 1 && z < ChunkDepth - 2;
		if (canFitTree && (static_cast<int>(globalX) % 29 == 4) && (static_cast<int>(globalZ) % 31 == 6))
		{
			AddTree(x, z, surfaceHeight, BlockId::OakLog, BlockId::OakLeaves, blocks);
		}
		else if (canFitTree && (static_cast<int>(globalX) % 43 == 10) && (static_cast<int>(globalZ) % 47 == 14))
		{
			AddTree(x, z, surfaceHeight, BlockId::BirchLog, BlockId::BirchLeaves, blocks);
		}
		else if (surfaceHeight < 61 && surfacePattern % 43 == 0) SetAboveSurface(x, z, surfaceHeight, BlockId::Pumpkin, blocks);
		else if (surfaceHeight < 57 && surfacePattern % 47 == 0) SetAboveSurface(x, z, surfaceHeight, BlockId::Melon, blocks);
		else if (surfacePattern % 53 == 0) SetAboveSurface(x, z, surfaceHeight, BlockId::HayBlock, blocks);
		else if (surfacePattern % 59 == 0) SetAboveSurface(x, z, surfaceHeight, BlockId::CraftingTable, blocks);
		else if (surfacePattern % 61 == 0) SetAboveSurface(x, z, surfaceHeight, BlockId::Bookshelf, blocks);
	}
}
