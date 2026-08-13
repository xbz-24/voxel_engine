#include <voxel/Authoring.h>

int main()
{
	constexpr voxel::SdkVersion version = voxel::Version();
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithWorld(voxel::World(7).AddFloor(0, 1, voxel::Grass));
	return version.major == 0 && version.minor == 5 && version.patch == 0 &&
		config.world.edits.size() == 1 ? 0 : 1;
}
