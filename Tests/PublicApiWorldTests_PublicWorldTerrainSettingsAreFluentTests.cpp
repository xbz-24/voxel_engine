#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public world terrain settings are fluent")
{
	const voxel::TerrainPalette palette{
		voxel::Obsidian,
		voxel::Blackstone,
		voxel::Sandstone,
		voxel::RedSand,
		voxel::RedSandstone,
		voxel::Snow,
		voxel::CutSandstone,
		voxel::Gravel
	};
	const voxel::WorldConfig world = voxel::World(6)
		.WithTerrainSeed(1234U)
		.WithBaseSurfaceHeight(22)
		.WithTerrainGenerator(voxel::TerrainGenerator::Flat)
		.WithTerrainBiome(voxel::TerrainBiome::Desert)
		.WithTerrainPalette(palette);

	CHECK(world.size_chunks == 6);
	CHECK(world.terrain_seed == 1234U);
	CHECK(world.base_surface_height == 22);
	CHECK(world.terrain_generator == voxel::TerrainGenerator::Flat);
	CHECK(world.terrain_biome == voxel::TerrainBiome::Desert);
	CHECK(world.terrain_palette.surface == voxel::RedSand);
	CHECK(world.terrain_palette.subsurface == voxel::RedSandstone);
}

