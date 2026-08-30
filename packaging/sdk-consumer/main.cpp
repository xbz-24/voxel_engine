#include <voxel/Voxel.h>

#include <filesystem>
#include <utility>

int main()
{
	voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithRuntimeLayout(voxel::RuntimeLayout{}
			.AssetsAt(std::filesystem::path{ "external/assets" })
			.VulkanShadersAt(std::filesystem::path{ "external/shaders/vulkan" }))
		.UseDiscoveredRuntimeLayout();
	voxel::Engine engine(std::move(config));
	const voxel::SdkVersion version = voxel::Version();
	return !engine.IsRunning() && version.major == 0 && version.minor == 5 &&
		version.patch == 0 ? 0 : 1;
}
