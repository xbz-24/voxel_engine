#include <voxel/Voxel.h>

int main()
{
	return voxel::RunScene(voxel::Scene()
		.AddPlatform(62, 10, voxel::Grass)
		.AddHouse(0, 63, 0)
		.AddGarden(-8, 63, 4)
		.AddPond(8, 63, 4));
}
