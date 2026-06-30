#include "ChunkTerrainPalette.h"

using ve::blocks::BlockId;

namespace ve::world::terrain::detail
{
	namespace
	{
		[[nodiscard]] bool UsesDefaultTerrainPalette(const ve::world::TerrainBlockPalette& block_palette)
		{
			const ve::world::TerrainBlockPalette default_block_palette{};
			return block_palette.bedrockBlock == default_block_palette.bedrockBlock &&
				block_palette.deepStoneBlock == default_block_palette.deepStoneBlock &&
				block_palette.stoneBlock == default_block_palette.stoneBlock &&
				block_palette.surfaceBlock == default_block_palette.surfaceBlock &&
				block_palette.subsurfaceBlock == default_block_palette.subsurfaceBlock &&
				block_palette.highElevationSurfaceBlock == default_block_palette.highElevationSurfaceBlock &&
				block_palette.drySurfacePatchBlock == default_block_palette.drySurfacePatchBlock &&
				block_palette.gravelPatchBlock == default_block_palette.gravelPatchBlock;
		}

		[[nodiscard]] ve::world::TerrainBlockPalette DesertTerrainPalette()
		{
			return {
				BlockId::Bedrock,
				BlockId::Blackstone,
				BlockId::Sandstone,
				BlockId::Sand,
				BlockId::Sandstone,
				BlockId::RedSand,
				BlockId::RedSand,
				BlockId::Gravel
			};
		}

		[[nodiscard]] ve::world::TerrainBlockPalette AlpineTerrainPalette()
		{
			return {
				BlockId::Bedrock,
				BlockId::Deepslate,
				BlockId::Stone,
				BlockId::Snow,
				BlockId::Dirt,
				BlockId::Snow,
				BlockId::Gravel,
				BlockId::Gravel
			};
		}
	}

	ve::world::TerrainBlockPalette ResolveTerrainPalette(
		const ve::world::TerrainGenerationSettings& terrain_generation)
	{
		if (!UsesDefaultTerrainPalette(terrain_generation.blockPalette))
		{
			return terrain_generation.blockPalette;
		}
		if (terrain_generation.spawnBiome == ve::world::TerrainSpawnBiome::Desert)
		{
			return DesertTerrainPalette();
		}
		if (terrain_generation.spawnBiome == ve::world::TerrainSpawnBiome::Alpine)
		{
			return AlpineTerrainPalette();
		}
		return terrain_generation.blockPalette;
	}
}
