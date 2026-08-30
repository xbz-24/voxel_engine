#include <voxel/Voxel.h>

#include <iostream>
#include <string>
#include <utility>

int main()
{
	auto config = voxel::EngineConfig::Default()
		.OnUpdate([](voxel::FrameContext& frame) {
			if (frame.elapsed_seconds >= 5.0f)
			{
				frame.commands.RequestClose();
			}
		})
		.OnDiagnostics([](const voxel::Diagnostics& diagnostics) {
			std::cout << "fps: " << diagnostics.fps << '\n';
		})
		.OnLog([](const std::string& message) {
			std::clog << message << '\n';
		});

	return voxel::Run(std::move(config));
}
