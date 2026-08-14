#pragma once

#include <voxel/Voxel.h>

#include <algorithm>

namespace world_load_test
{
	inline bool SamePosition(voxel::BlockPosition left, voxel::BlockPosition right)
	{
		return left.x == right.x && left.y == right.y && left.z == right.z;
	}

	inline bool SameBox(const voxel::BlockBox& left, const voxel::BlockBox& right)
	{
		return SamePosition(left.minimum, right.minimum) &&
			SamePosition(left.maximum, right.maximum);
	}

	inline bool SameEdit(const voxel::WorldEdit& left, const voxel::WorldEdit& right)
	{
		return left.kind == right.kind && left.block == right.block &&
			SamePosition(left.position, right.position) && SameBox(left.box, right.box);
	}

	inline bool SamePalette(
		const voxel::TerrainPalette& left, const voxel::TerrainPalette& right)
	{
		return left.bedrock == right.bedrock && left.deep_stone == right.deep_stone &&
			left.stone == right.stone && left.surface == right.surface &&
			left.subsurface == right.subsurface &&
			left.high_elevation_surface == right.high_elevation_surface &&
			left.dry_surface_patch == right.dry_surface_patch &&
			left.gravel_patch == right.gravel_patch;
	}

	inline bool SameWorld(const voxel::WorldConfig& left, const voxel::WorldConfig& right)
	{
		return left.size_chunks == right.size_chunks &&
			left.terrain_seed == right.terrain_seed &&
			left.base_surface_height == right.base_surface_height &&
			left.terrain_generator == right.terrain_generator &&
			left.terrain_biome == right.terrain_biome &&
			SamePalette(left.terrain_palette, right.terrain_palette) &&
			left.edits.size() == right.edits.size() &&
			std::equal(left.edits.begin(), left.edits.end(), right.edits.begin(), SameEdit);
	}

	inline voxel::WorldConfig SentinelWorld()
	{
		return voxel::World(13)
			.WithTerrainSeed(4'000'000'001U)
			.WithBaseSurfaceHeight(-17)
			.WithTerrainGenerator(voxel::TerrainGenerator::Flat)
			.WithTerrainBiome(voxel::TerrainBiome::Alpine)
			.WithTerrainPalette(voxel::TerrainPalette{
				voxel::Obsidian, voxel::Blackstone, voxel::Granite, voxel::Snow,
				voxel::Dirt, voxel::Andesite, voxel::RedSand, voxel::Gravel
			})
			.SetBlock(-9, 8, 7, voxel::DiamondOre)
			.FillBox(-4, -3, -2, 1, 2, 3, voxel::SeaLantern);
	}
}
