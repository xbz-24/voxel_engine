#include "DemoCommandLine.h"
#include "DemoConfig.h"
#include "ResizeSmokeController.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
	[[nodiscard]] std::vector<std::string_view> CaptureArguments(int argc, char** argv)
	{
		std::vector<std::string_view> arguments;
		if (argc <= 0 || argv == nullptr) return arguments;
		arguments.reserve(static_cast<std::size_t>(argc));
		for (int index = 0; index < argc; ++index)
		{
			arguments.emplace_back(argv[index] == nullptr ? std::string_view{} : std::string_view{ argv[index] });
		}
		return arguments;
	}
}

int main(int argc, char** argv)
{
	const std::vector<std::string_view> arguments = CaptureArguments(argc, argv);
	const voxel_demo::DemoOptions options = voxel_demo::ParseOptions(arguments);
	if (!options.valid) return 2;

	voxel::EngineConfig config = voxel_demo::CreateDemoConfig(options.graphics_api);
	if (!options.asset_directory.empty())
	{
		voxel::RuntimeLayout layout{};
		layout.AssetsAt(std::filesystem::path{ std::string{ options.asset_directory } });
		if (!options.vulkan_shader_directory.empty())
			layout.VulkanShadersAt(
				std::filesystem::path{ std::string{ options.vulkan_shader_directory } });
		config.WithRuntimeLayout(std::move(layout));
	}
	voxel_demo::ResizeSmokeController resize_smoke{
		options.resize_smoke_frame, options.smoke_frame_limit };
	if (options.resize_smoke_frame > 0)
	{
		config.HideDebugOverlay().OnUpdate(
			[&resize_smoke](voxel::FrameContext& frame) { resize_smoke.OnFrame(frame); });
	}
	else if (options.smoke_frame_limit > 0)
	{
		config.HideDebugOverlay().OnUpdate(
			[frame_limit = options.smoke_frame_limit, frame_count = 0](voxel::FrameContext& frame) mutable {
				if (++frame_count >= frame_limit) frame.commands.RequestClose();
			});
	}
	const int runtime_result = voxel::Run(std::move(config));
	if (runtime_result != 0 || options.resize_smoke_frame == 0) return runtime_result;
	return resize_smoke.CompletedSuccessfully() ? 0 : 3;
}
