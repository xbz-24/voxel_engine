#include <voxel/Voxel.h>

int main()
{
	const voxel::EngineConfig config = voxel::DesertDemo()
		.WithWindow(voxel::Window("SDK Header Smoke", 640, 480))
		.WithWorld(voxel::Scene()
			.WithSizeChunks(4)
			.AddFloor(63, 3, voxel::Grass)
			.AddTree(0, 64, 0))
		.HideDebugOverlay();

	return config.IsValid() ? 0 : 1;
}
