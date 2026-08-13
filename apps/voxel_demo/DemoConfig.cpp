#include "DemoConfig.h"

namespace voxel_demo
{
	namespace
	{
		[[nodiscard]] voxel::WorldConfig CreateSandboxWorld()
		{
			voxel::TerrainPalette desert_palette{};
			desert_palette.surface = voxel::Sand;
			desert_palette.subsurface = voxel::Sandstone;
			desert_palette.high_elevation_surface = voxel::RedSand;
			desert_palette.dry_surface_patch = voxel::Terracotta;
			desert_palette.gravel_patch = voxel::RedSandstone;

			return voxel::Scene(16)
				.WithTerrainSeed(6407)
				.WithBaseSurfaceHeight(44)
				.WithTerrainBiome(voxel::TerrainBiome::Desert)
				.WithTerrainPalette(desert_palette)
				.AddPlatform(54, 18, voxel::CutSandstone)
				.AddPathZ(0, 55, -24, 24, 2, voxel::RedSandstone)
				.AddPathX(-24, 24, 55, 0, 2, voxel::RedSandstone)
				.AddHouse(-11, 55, -11, 5, voxel::Sandstone, voxel::Terracotta)
				.AddHouse(11, 55, -11, 4, voxel::CutSandstone, voxel::RedSandstone)
				.AddPond(0, 55, 12, 6, voxel::Water, voxel::MossyCobblestone)
				.AddGarden(-12, 55, 11, 4, 5, voxel::Dirt, voxel::HayBlock)
				.AddLightPost(-6, 55, -6)
				.AddLightPost(6, 55, -6)
				.AddLightPost(-6, 55, 6)
				.AddLightPost(6, 55, 6)
				.AddTree(14, 55, 13)
				.AddTree(-16, 55, 14, voxel::BirchLog, voxel::BirchLeaves)
				.AddColumn(0, 55, 0, 10, voxel::Obsidian)
				.AddLamp(0, 65, 0);
		}

		[[nodiscard]] voxel::VoxelRenderStyle CreateRenderStyle()
		{
			return voxel::VoxelRenderStyle{}
				.WithSunDirection({ -0.32f, 0.82f, -0.46f })
				.WithSunColor({ 1.30f, 1.08f, 0.82f })
				.WithSkyColors({ 0.70f, 0.66f, 0.58f }, { 0.30f, 0.52f, 0.86f })
				.WithExposure(1.05f)
				.WithFogRange(120.0f, 420.0f)
				.WithFogStrength(0.40f)
				.WithClouds(0.38f, 0.58f, 0.016f)
				.WithSurfaceDetail(1.18f)
				.WithWaterReflections(1.25f)
				.WithAmbientOcclusion(1.08f)
				.WithAerialPerspective(1.12f)
				.WithDirectionalShadows(0.95f, 1.10f, 240.0f);
		}
	}

	voxel::EngineConfig CreateDemoConfig(voxel::GraphicsApi graphics_api)
	{
		return voxel::EngineConfig::Default()
			.UseGraphicsApi(graphics_api)
			.WithWindow("Voxel Sandbox", 1600, 900)
			.WithWorld(CreateSandboxWorld())
			.WithCamera({ 32.0f, 78.0f, 38.0f }, { 0.0f, 56.0f, 0.0f })
			.WithVoxelRenderStyle(CreateRenderStyle())
			.WithRenderDistanceChunks(12)
			.ShowDebugOverlay(true);
	}
}
