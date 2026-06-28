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

	return voxel::Run(voxel::DesertDemo()
		.WithWindow("Voxel API - Custom Scene", 1280, 720)
		.WithScene(scene)
		.WithCamera({ 18.0f, 74.0f, 22.0f }, { 0.0f, 64.0f, 0.0f })
		.HideDebugOverlay());
}
