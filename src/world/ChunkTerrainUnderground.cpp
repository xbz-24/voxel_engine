#include "ChunkTerrainUnderground.h"

using ve::blocks::BlockId;

namespace ve::world::terrain::detail
{
	BlockId UndergroundBlock(
		int local_block_y,
		float world_block_x,
		float world_block_z,
		const ve::world::TerrainBlockPalette& block_palette,
		const ve::world::TerrainGenerationSettings& terrain_generation)
	{
		const int seed_pattern = static_cast<int>(terrain_generation.terrainSeed % 8191U);
		const int ore_pattern = static_cast<int>(
			world_block_x * 13.0f + world_block_z * 7.0f + static_cast<float>(local_block_y) * 5.0f) +
			seed_pattern;
		if (local_block_y == 0) return block_palette.bedrockBlock;
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
		return local_block_y < 18 ? block_palette.deepStoneBlock : block_palette.stoneBlock;
	}
}
