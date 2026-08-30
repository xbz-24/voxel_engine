#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public world serialization roundtrips config edits")
{
	const auto uniquePathSuffix = std::chrono::steady_clock::now().time_since_epoch().count();
	const std::filesystem::path path = std::filesystem::temp_directory_path() /
		("voxel_public_world_roundtrip_" + std::to_string(uniquePathSuffix) + ".voxelscene");
	const voxel::WorldConfig original = voxel::World(5)
		.WithTerrainSeed(77U)
		.WithBaseSurfaceHeight(23)
		.WithTerrainGenerator(voxel::TerrainGenerator::Flat)
		.WithTerrainBiome(voxel::TerrainBiome::Alpine)
		.WithTerrainPalette(voxel::TerrainPalette{
			voxel::Obsidian,
			voxel::Blackstone,
			voxel::Stone,
			voxel::Snow,
			voxel::Dirt,
			voxel::Snow,
			voxel::Gravel,
			voxel::Andesite
		})
		.SetBlock(1, 2, 3, voxel::DiamondOre)
		.FillBox(-1, 0, -1, 1, 0, 1, voxel::Grass);

	REQUIRE(voxel::SaveWorldConfig(original, path.string()));
	const voxel::WorldConfig loaded = voxel::LoadWorldConfig(path.string());
	std::error_code cleanupError;
	std::filesystem::remove(path, cleanupError);

	CHECK(loaded.size_chunks == 5);
	CHECK(loaded.terrain_seed == 77U);
	CHECK(loaded.base_surface_height == 23);
	CHECK(loaded.terrain_generator == voxel::TerrainGenerator::Flat);
	CHECK(loaded.terrain_biome == voxel::TerrainBiome::Alpine);
	CHECK(loaded.terrain_palette.bedrock == voxel::Obsidian);
	CHECK(loaded.terrain_palette.deep_stone == voxel::Blackstone);
	CHECK(loaded.terrain_palette.stone == voxel::Stone);
	CHECK(loaded.terrain_palette.surface == voxel::Snow);
	CHECK(loaded.terrain_palette.subsurface == voxel::Dirt);
	CHECK(loaded.terrain_palette.high_elevation_surface == voxel::Snow);
	CHECK(loaded.terrain_palette.dry_surface_patch == voxel::Gravel);
	CHECK(loaded.terrain_palette.gravel_patch == voxel::Andesite);
	REQUIRE(loaded.edits.size() == 2);
	CHECK(loaded.edits[0].kind == voxel::WorldEdit::Kind::SetBlock);
	CHECK(loaded.edits[0].position.x == 1);
	CHECK(loaded.edits[0].block == voxel::DiamondOre);
	CHECK(loaded.edits[1].kind == voxel::WorldEdit::Kind::FillBox);
	CHECK(loaded.edits[1].box.minimum.x == -1);
	CHECK(loaded.edits[1].block == voxel::Grass);
}
