#include <doctest/doctest.h>

#include <voxel/EngineConfig.h>

#include <algorithm>
#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace
{
	[[nodiscard]] bool ContainsIssue(const std::vector<std::string>& issues, std::string_view expected)
	{
		return std::ranges::find(issues, expected) != issues.end();
	}
}

TEST_CASE("public config validation rejects unknown startup enum values")
{
	voxel::EngineConfig config = voxel::EngineConfig::Default();
	config.graphics_api = static_cast<voxel::GraphicsApi>(255);
	config.logging.minimum_level = static_cast<voxel::LogLevel>(255);
	config.world.terrain_generator = static_cast<voxel::TerrainGenerator>(255);
	config.world.terrain_biome = static_cast<voxel::TerrainBiome>(255);
	config.world.terrain_palette.surface = static_cast<voxel::Block>(255);

	const std::vector<std::string> issues = config.Validate();
	CHECK(ContainsIssue(issues, "graphics_api is not a known graphics api"));
	CHECK(ContainsIssue(issues, "logging.minimum_level is not a known log level"));
	CHECK(ContainsIssue(issues,
		"world.terrain_generator is not a known terrain generator"));
	CHECK(ContainsIssue(issues, "world.terrain_biome is not a known terrain biome"));
	CHECK(ContainsIssue(issues, "world.terrain_palette contains an unknown block"));
	CHECK_FALSE(config.IsValid());
}

TEST_CASE("public terrain palette validation covers every block slot")
{
	using PaletteMember = voxel::Block voxel::TerrainPalette::*;
	constexpr std::array<PaletteMember, 8> members{{
		&voxel::TerrainPalette::bedrock,
		&voxel::TerrainPalette::deep_stone,
		&voxel::TerrainPalette::stone,
		&voxel::TerrainPalette::surface,
		&voxel::TerrainPalette::subsurface,
		&voxel::TerrainPalette::high_elevation_surface,
		&voxel::TerrainPalette::dry_surface_patch,
		&voxel::TerrainPalette::gravel_patch
	}};
	for (PaletteMember member : members)
	{
		voxel::EngineConfig config = voxel::EngineConfig::Default();
		config.world.terrain_palette.*member = static_cast<voxel::Block>(255);
		CHECK(ContainsIssue(config.Validate(),
			"world.terrain_palette contains an unknown block"));
	}
}
