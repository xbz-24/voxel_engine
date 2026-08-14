#include "StaticSceneSmokeScenarios.h"

#include "StaticSceneSmokeSupport.h"

#include <stdexcept>
#include <string>

namespace static_scene_smoke
{
	int RunBoundedScene(const std::filesystem::path& assets,
		const std::filesystem::path& model_path)
	{
		const int result = voxel::Run(CreateConfig(assets, model_path,
			[frames = 0](voxel::FrameContext& frame) mutable {
				if (++frames >= 3) frame.commands.RequestClose();
			}));
		return result == 0 ? 0 : 10;
	}

	int RunExceptionRecovery(const std::filesystem::path& assets,
		const std::filesystem::path& model_path)
	{
		voxel::Engine* active_engine = nullptr;
		bool teardown_observed_not_running = false;
		voxel::Engine engine{ CreateConfig(assets, model_path,
			[](voxel::FrameContext&) { throw std::runtime_error("frame failure"); },
			[&](const std::string& line) {
				if (active_engine != nullptr &&
					line.find("Engine runtime stopped") != std::string::npos)
				{
					teardown_observed_not_running = !active_engine->IsRunning();
				}
			}) };
		active_engine = &engine;
		try
		{
			if (!engine.Start()) return 20;
			static_cast<void>(engine.Step());
			return 21;
		}
		catch (const std::runtime_error&)
		{
			if (engine.IsRunning()) return 22;
			if (!teardown_observed_not_running) return 26;
			if (!engine.Start()) return 23;
			engine.Shutdown();
			return engine.IsRunning() ? 24 : 0;
		}
		catch (...) { return 25; }
	}
}
