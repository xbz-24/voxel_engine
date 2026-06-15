#include "ChunkTerrain.h"

#include "ChunkDecorations.h"

#include <algorithm>
#include <cmath>

using ve::blocks::BlockId;

namespace ve::world::terrain
{
	namespace
	{
		/**
		 * Computes the terrain height from a few cheap wave layers.
		 */
		int SurfaceHeight(float globalX, float globalZ)
		{
			const float continent = std::sin(globalX * 0.01f) * std::cos(globalZ * 0.01f) * 30.0f;
			const float mountains = std::sin(globalX * 0.05f + globalZ * 0.02f) * 15.0f;
			const float hills = std::sin(globalX * 0.15f) * std::cos(globalZ * 0.15f) * 4.0f;
			return std::clamp(40 + static_cast<int>(continent + mountains + hills), 1, ChunkHeight - 2);
		}

		/**
		 * Chooses stone or ore for buried blocks.
		 */
		BlockId UndergroundBlock(int y, float globalX, float globalZ)
		{
			const int orePattern = static_cast<int>(globalX * 13.0f + globalZ * 7.0f + static_cast<float>(y) * 5.0f);
			if (y == 0) return BlockId::Bedrock;
			if (y < 28 && orePattern % 97 == 0) return BlockId::DiamondOre;
			if (y < 34 && orePattern % 89 == 0) return BlockId::EmeraldOre;
			if (y < 38 && orePattern % 79 == 0) return BlockId::LapisOre;
			if (y < 36 && orePattern % 71 == 0) return BlockId::GoldOre;
			if (y < 42 && orePattern % 53 == 0) return BlockId::IronOre;
			if (y < 55 && orePattern % 47 == 0) return BlockId::CopperOre;
			if (orePattern % 37 == 0) return BlockId::CoalOre;
			if (y < 16 && orePattern % 31 == 0) return BlockId::Blackstone;
			if (y < 22 && orePattern % 43 == 0) return BlockId::Basalt;
			if (y < 30 && orePattern % 67 == 0) return BlockId::AmethystBlock;
			if (orePattern % 29 == 0) return BlockId::Granite;
			if (orePattern % 23 == 0) return BlockId::Diorite;
			if (orePattern % 19 == 0) return BlockId::Andesite;
			return y < 18 ? BlockId::Deepslate : BlockId::Stone;
		}

	}

	void GenerateChunkTerrain(int chunkX, int chunkZ, BlockStorage& blocks)
	{
		for (int x = 0; x < ChunkWidth; x++)
		{
			for (int z = 0; z < ChunkDepth; z++)
			{
				const float globalX = static_cast<float>(x + (chunkX * ChunkWidth));
				const float globalZ = static_cast<float>(z + (chunkZ * ChunkDepth));
				const int surfaceHeight = SurfaceHeight(globalX, globalZ);
				const int patchPattern = static_cast<int>(globalX * 3.0f + globalZ * 11.0f);
				const int surfacePattern = static_cast<int>(globalX * 5.0f + globalZ * 9.0f);

				for (int y = 0; y < ChunkHeight; y++)
				{
					if (y < surfaceHeight - 4) blocks[x][y][z] = UndergroundBlock(y, globalX, globalZ);
					else if (y < surfaceHeight) blocks[x][y][z] = patchPattern % 19 == 0 ? BlockId::Gravel : BlockId::Dirt;
					else if (y == surfaceHeight) blocks[x][y][z] = surfaceHeight > 62 ? BlockId::Snow : (surfacePattern % 29 == 0 ? BlockId::Sand : (surfacePattern % 31 == 0 ? BlockId::MossBlock : BlockId::Grass));
					else blocks[x][y][z] = BlockId::Air;
				}

				decorations::ApplySurfaceDecorations(x, z, surfaceHeight, globalX, globalZ, surfacePattern, blocks);
			}
		}
	}
}
