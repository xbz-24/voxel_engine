#include <voxel/Voxel.h>

int main()
{
	voxel::Engine engine(voxel::EngineConfig::Default());
	const voxel::SdkVersion version = voxel::Version();
	return !engine.IsRunning() && version.major == 0 && version.minor == 5 &&
		version.patch == 0 ? 0 : 1;
}
