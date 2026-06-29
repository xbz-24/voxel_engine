#include "ChunkTerrainColumn.h"

#include <algorithm>
#include <cmath>

using ve::blocks::BlockId;

namespace ve::world::terrain::detail
{
	namespace
	{
		[[nodiscard]] int SurfaceHeight(float world_block_x, float world_block_z)
		{
			const float continent = std::sin(world_block_x * 0.01f) * std::cos(world_block_z * 0.01f) * 30.0f;
			const float mountains = std::sin(world_block_x * 0.05f + world_block_z * 0.02f) * 15.0f;
			const float hills = std::sin(world_block_x * 0.15f) * std::cos(world_block_z * 0.15f) * 4.0f;
			return std::clamp(40 + static_cast<int>(continent + mountains + hills), 1, ChunkHeight - 2);
		}

		[[nodiscard]] BlockId UndergroundBlock(int local_block_y, float world_block_x, float world_block_z)
		{
			const int ore_pattern = static_cast<int>(
				world_block_x * 13.0f + world_block_z * 7.0f + static_cast<float>(local_block_y) * 5.0f);
			if (local_block_y == 0) return BlockId::Bedrock;
			if (local_block_y < 28 && ore_pattern % 97 == 0) return BlockId::DiamondOre;
			if (local_block_y < 34 && ore_pattern % 89 == 0) return BlockId::EmeraldOre;
			if (local_block_y < 38 && ore_pattern % 79 == 0) return BlockId::LapisOre;
			if (local_block_y < 36 && ore_pattern % 71 == 0) return BlockId::GoldOre;
			if (local_block_y < 42 && ore_pattern % 53 == 0) return BlockId::IronOre;
			if (local_block_y < 55 && ore_pattern % 47 == 0) return BlockId::CopperOre;
			if (ore_pattern % 37 == 0) return BlockId::CoalOre;
			if (local_block_y < 16 && ore_pattern % 31 == 0) return BlockId::Blackstone;
			if (local_block_y < 22 && ore_pattern % 43 == 0) return BlockId::Basalt;
			if (local_block_y < 30 && ore_pattern % 67 == 0) return BlockId::AmethystBlock;
			if (ore_pattern % 29 == 0) return BlockId::Granite;
			if (ore_pattern % 23 == 0) return BlockId::Diorite;
			if (ore_pattern % 19 == 0) return BlockId::Andesite;
			return local_block_y < 18 ? BlockId::Deepslate : BlockId::Stone;
		}

		[[nodiscard]] BlockId SurfaceBlock(const TerrainColumn& column)
		{
			if (column.surface_y > 62) return BlockId::Snow;
			if (column.surface_block_pattern % 29 == 0) return BlockId::Sand;
			if (column.surface_block_pattern % 31 == 0) return BlockId::MossBlock;
			return BlockId::Grass;
		}

		[[nodiscard]] BlockId BlockForDepth(const TerrainColumn& column, int local_block_y)
		{
			if (local_block_y < column.surface_y - 4)
			{
				return UndergroundBlock(local_block_y, column.world_block_x, column.world_block_z);
			}
			if (local_block_y < column.surface_y)
			{
				return column.ground_patch_pattern % 19 == 0 ? BlockId::Gravel : BlockId::Dirt;
			}
			if (local_block_y == column.surface_y) return SurfaceBlock(column);
			return BlockId::Air;
		}
	}

	TerrainColumn BuildTerrainColumn(int chunk_x, int chunk_z, int local_block_x, int local_block_z)
	{
		const float world_block_x = static_cast<float>(local_block_x + (chunk_x * ChunkWidth));
		const float world_block_z = static_cast<float>(local_block_z + (chunk_z * ChunkDepth));
		return {
			local_block_x,
			local_block_z,
			world_block_x,
			world_block_z,
			SurfaceHeight(world_block_x, world_block_z),
			static_cast<int>(world_block_x * 3.0f + world_block_z * 11.0f),
			static_cast<int>(world_block_x * 5.0f + world_block_z * 9.0f)
		};
	}

	void FillTerrainColumn(const TerrainColumn& column, BlockStorage& blocks)
	{
		for (int local_block_y = 0; local_block_y < ChunkHeight; ++local_block_y)
		{
			blocks[column.local_block_x][local_block_y][column.local_block_z] =
				BlockForDepth(column, local_block_y);
		}
	}
}
