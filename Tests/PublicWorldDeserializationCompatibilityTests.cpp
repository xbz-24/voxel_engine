#include "WorldLoadTestSupport.h"

#include <doctest/doctest.h>

using namespace world_load_test;

TEST_CASE("strict world loading accepts legacy v1 records without terrain fields")
{
	const TemporaryWorldPath path(
		"voxel-world-config 1\r\n\r\n"
		"size 3\r\n"
		"set -1 2 3 3\r\n"
		"set 4 5 6 9\r\n"
		"\r\n"
		"fill -2 0 -2 2 0 2 1\r\n"
		"fill 7 8 9 10 11 12 0\r\n");
	voxel::WorldConfig output = SentinelWorld();
	const voxel::WorldConfig expected = voxel::World(3)
		.SetBlock(-1, 2, 3, voxel::Stone)
		.SetBlock(4, 5, 6, voxel::DiamondOre)
		.FillBox(-2, 0, -2, 2, 0, 2, voxel::Grass)
		.FillBox(7, 8, 9, 10, 11, 12, voxel::Air);

	REQUIRE(voxel::TryLoadWorldConfig(path.Get().string(), output));
	CHECK(SameWorld(output, expected));
}

TEST_CASE("strict world loading accepts complete terrain fields in free order")
{
	const TemporaryWorldPath path(
		"voxel-world-config 1\n"
		"terrain_biome alpine\n"
		"terrain_palette 12 26 23 21 2 24 48 19\n"
		"size 6\n"
		"terrain_seed 99\n"
		"terrain_generator flat\n"
		"base_surface_height -5\n");
	voxel::WorldConfig output = SentinelWorld();
	const voxel::WorldConfig expected = voxel::World(6)
		.WithTerrainSeed(99U)
		.WithBaseSurfaceHeight(-5)
		.WithTerrainGenerator(voxel::TerrainGenerator::Flat)
		.WithTerrainBiome(voxel::TerrainBiome::Alpine)
		.WithTerrainPalette(voxel::TerrainPalette{
			voxel::Obsidian, voxel::Blackstone, voxel::Granite, voxel::Snow,
			voxel::Dirt, voxel::Andesite, voxel::RedSand, voxel::Gravel
		});

	REQUIRE(voxel::TryLoadWorldConfig(path.Get().string(), output));
	CHECK(SameWorld(output, expected));
}

TEST_CASE("legacy world loading returns a default world after strict failure")
{
	const TemporaryWorldPath path(
		"voxel-world-config 1\n"
		"size 4\n"
		"terrain_generator unknown\n");

	CHECK(SameWorld(voxel::LoadWorldConfig(path.Get().string()), voxel::WorldConfig{}));
}
