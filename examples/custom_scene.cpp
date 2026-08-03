#include <voxel/Voxel.h>

int main()
{
	voxel::WorldConfig scene = voxel::Scene()
		.AddPlatform(62, 14, voxel::Grass)
		.AddHouse(0, 63, 0)
		.AddPathZ(0, 63, -12, -4)
		.AddBridgeX(5, 12, 64, 5)
		.AddPond(10, 63, 5)
		.AddGarden(-10, 63, 5)
		.AddLightPost(-4, 63, -7)
		.AddLightPost(4, 63, -7)
		.AddTree(-9, 63, -7)
		.AddTree(8, 63, 8, voxel::BirchLog, voxel::BirchLeaves)
		.AddLamp(0, 68, 0);

	return voxel::Run(voxel::EngineConfig::Default()
		.WithWindow("Voxel API - Custom Scene", 1280, 720)
		.WithScene(scene)
		.WithCamera({ 18.0f, 74.0f, 22.0f }, { 0.0f, 64.0f, 0.0f })
		.WithVoxelRenderStyle(voxel::VoxelRenderStyle{}
			.WithSunDirection({ -0.30f, 0.82f, -0.48f })
			.WithSunColor({ 1.30f, 1.08f, 0.82f })
			.WithSkyColors({ 0.66f, 0.72f, 0.78f }, { 0.28f, 0.52f, 0.86f })
			.WithExposure(1.05f)
			.WithFogRange(110.0f, 380.0f)
			.WithFogStrength(0.48f)
			.WithClouds(0.44f, 0.64f, 0.018f)
			.WithSurfaceDetail(1.15f)
			.WithWaterReflections(1.25f)
			.WithAmbientOcclusion(1.05f)
			.WithAerialPerspective(1.10f)
			.WithSubsurfaceScattering(1.20f)
			.WithCloudLightAbsorption(1.08f)
			.WithDirectionalShadows(0.95f, 1.20f, 210.0f))
		.HideDebugOverlay());
}
