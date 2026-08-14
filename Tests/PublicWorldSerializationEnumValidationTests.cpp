#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace
{
	constexpr const char* Sentinel = "existing-world-data\n";

	std::filesystem::path TemporaryWorldPath()
	{
		const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
		return std::filesystem::temp_directory_path() /
			("voxel_invalid_world_" + std::to_string(suffix) + ".voxelscene");
	}

	void CheckRejectedWithoutOverwrite(
		const voxel::WorldConfig& world, const std::filesystem::path& path)
	{
		{
			std::ofstream seed(path, std::ios::trunc);
			REQUIRE(seed);
			seed << Sentinel;
			REQUIRE(seed.good());
		}
		CHECK_FALSE(voxel::SaveWorldConfig(world, path.string()));
		std::ifstream saved(path);
		REQUIRE(saved);
		const std::string contents{
			std::istreambuf_iterator<char>{saved}, std::istreambuf_iterator<char>{}};
		CHECK(contents == Sentinel);
	}

	voxel::WorldConfig SerializableWorld()
	{
		return voxel::World(1).SetBlock(1, 2, 3, voxel::Stone);
	}
}

TEST_CASE("world serialization preserves existing files for unknown enums")
{
	const std::filesystem::path path = TemporaryWorldPath();
	voxel::WorldConfig world = SerializableWorld();
	world.terrain_generator = static_cast<voxel::TerrainGenerator>(255);
	CheckRejectedWithoutOverwrite(world, path);

	world = SerializableWorld();
	world.terrain_biome = static_cast<voxel::TerrainBiome>(255);
	CheckRejectedWithoutOverwrite(world, path);

	world = SerializableWorld();
	world.edits.front().kind = static_cast<voxel::WorldEdit::Kind>(255);
	CheckRejectedWithoutOverwrite(world, path);

	world = SerializableWorld();
	world.edits.front().block = static_cast<voxel::Block>(255);
	CheckRejectedWithoutOverwrite(world, path);
	std::error_code cleanup_error;
	std::filesystem::remove(path, cleanup_error);
}

TEST_CASE("world serialization checks every terrain palette block")
{
	using PaletteMember = voxel::Block voxel::TerrainPalette::*;
	constexpr std::array<PaletteMember, 8> members{{
		&voxel::TerrainPalette::bedrock, &voxel::TerrainPalette::deep_stone,
		&voxel::TerrainPalette::stone, &voxel::TerrainPalette::surface,
		&voxel::TerrainPalette::subsurface,
		&voxel::TerrainPalette::high_elevation_surface,
		&voxel::TerrainPalette::dry_surface_patch,
		&voxel::TerrainPalette::gravel_patch
	}};
	const std::filesystem::path path = TemporaryWorldPath();
	for (PaletteMember member : members)
	{
		voxel::WorldConfig world = SerializableWorld();
		world.terrain_palette.*member = static_cast<voxel::Block>(255);
		CheckRejectedWithoutOverwrite(world, path);
	}
	std::error_code cleanup_error;
	std::filesystem::remove(path, cleanup_error);
}
