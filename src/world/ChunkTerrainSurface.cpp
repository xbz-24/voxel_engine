#include "ChunkTerrainSurface.h"

#include "ChunkTerrainUnderground.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

using ve::blocks::BlockId;

namespace ve::world::terrain::detail
{
	namespace
	{
		[[nodiscard]] float SeedOffset(std::uint32_t terrain_seed, std::uint32_t channel) noexcept
		{
			const std::uint32_t mixed_seed = (terrain_seed * 1664525U) + (channel * 1013904223U);
			return static_cast<float>(mixed_seed % 4096U) * 0.03125f;
		}

		[[nodiscard]] BlockId SurfaceBlock(
			const TerrainColumn& column,
			const ve::world::TerrainBlockPalette& block_palette,
			const ve::world::TerrainGenerationSettings& terrain_generation)
		{
			const int high_elevation_threshold =
				terrain_generation.spawnBiome == ve::world::TerrainSpawnBiome::Alpine ? 48 : 62;
			if (column.surface_y > high_elevation_threshold) return block_palette.highElevationSurfaceBlock;
			if (column.surface_block_pattern % 29 == 0) return block_palette.drySurfacePatchBlock;
			if (column.surface_block_pattern % 31 == 0 &&
				terrain_generation.spawnBiome == ve::world::TerrainSpawnBiome::Temperate)
			{
				return BlockId::MossBlock;
			}
			return block_palette.surfaceBlock;
		}
	}

	int SurfaceHeight(
		float world_block_x,
		float world_block_z,
		const ve::world::TerrainGenerationSettings& terrain_generation)
	{
		if (terrain_generation.terrainGenerator == ve::world::TerrainGeneratorKind::Flat)
		{
			return std::clamp(terrain_generation.baseSurfaceHeight, 1, ChunkHeight - 2);
		}
		const float seeded_world_block_x =
			world_block_x + SeedOffset(terrain_generation.terrainSeed, 1U);
		const float seeded_world_block_z =
			world_block_z + SeedOffset(terrain_generation.terrainSeed, 2U);
		const float continent =
			std::sin(seeded_world_block_x * 0.01f) * std::cos(seeded_world_block_z * 0.01f) * 30.0f;
		const float mountains =
			std::sin(seeded_world_block_x * 0.05f + seeded_world_block_z * 0.02f) * 15.0f;
		const float hills =
			std::sin(seeded_world_block_x * 0.15f) * std::cos(seeded_world_block_z * 0.15f) * 4.0f;
		return std::clamp(
			terrain_generation.baseSurfaceHeight + static_cast<int>(continent + mountains + hills),
			1,
			ChunkHeight - 2);
	}

	BlockId BlockForDepth(
		const TerrainColumn& column,
		int local_block_y,
		const ve::world::TerrainBlockPalette& block_palette,
		const ve::world::TerrainGenerationSettings& terrain_generation)
	{
		if (local_block_y < column.surface_y - 4)
		{
			return UndergroundBlock(
				local_block_y,
				column.world_block_x,
				column.world_block_z,
				block_palette,
				terrain_generation);
		}
		if (local_block_y < column.surface_y)
		{
			return column.ground_patch_pattern % 19 == 0 ?
				block_palette.gravelPatchBlock :
				block_palette.subsurfaceBlock;
		}
		if (local_block_y == column.surface_y)
		{
			return SurfaceBlock(column, block_palette, terrain_generation);
		}
		return BlockId::Air;
	}
}
