#include "StaticSceneSmokeScenarios.h"

#include "StaticSceneSmokeSupport.h"

#include <string>

namespace static_scene_smoke
{
	namespace
	{
		struct ReentrantState
		{
			voxel::Engine* engine = nullptr;
			int session = 0;
			int updates = 0;
			int stop_logs = 0;
			bool startup_cancel_requested = false;
			bool nested_step = true;
			int nested_run = 0;
			bool observed_not_running = false;
			bool teardown_calls_rejected = true;
		};

		void OnUpdate(ReentrantState& state, voxel::FrameContext& frame)
		{
			++state.updates;
			if (state.session == 1)
			{
				state.nested_step = state.engine->Step();
				state.nested_run = state.engine->Run();
				state.engine->Shutdown();
				state.engine->Shutdown();
				state.observed_not_running = !state.engine->IsRunning();
			}
			else if (state.session == 2) frame.commands.RequestClose();
		}

		void OnLog(ReentrantState& state, const std::string& line)
		{
			if (state.session == 0 && !state.startup_cancel_requested &&
				line.find("Engine runtime started") != std::string::npos)
			{
				state.startup_cancel_requested = true;
				state.engine->Shutdown();
			}
			if (line.find("Engine runtime stopped") == std::string::npos) return;
			++state.stop_logs;
			bool calls_rejected = false;
			try
			{
				calls_rejected = !state.engine->Step() && state.engine->Run() == -1;
			}
			catch (...) {}
			state.teardown_calls_rejected = state.teardown_calls_rejected && calls_rejected;
			state.engine->Shutdown();
		}
	}

	int RunReentrantLifecycle(const std::filesystem::path& assets,
		const std::filesystem::path& model_path)
	{
		ReentrantState state{};
		voxel::Engine engine{ CreateConfig(assets, model_path,
			[&state](voxel::FrameContext& frame) { OnUpdate(state, frame); },
			[&state](const std::string& line) { OnLog(state, line); }) };
		state.engine = &engine;
		const voxel::EngineStartResult cancelled_start = engine.StartDetailed();
		if (cancelled_start.failure != voxel::EngineStartFailure::RuntimeStartupFailed ||
			cancelled_start.message.find("cancelled") == std::string::npos || engine.IsRunning()) return 30;
		if (!state.startup_cancel_requested || state.stop_logs != 1) return 31;

		state.session = 1;
		if (engine.Run() != 0 || engine.IsRunning()) return 32;
		if (state.updates != 1 || state.nested_step || state.nested_run != -1) return 33;
		if (!state.observed_not_running || state.stop_logs != 2) return 34;

		state.session = 2;
		if (!engine.Start() || !engine.IsRunning()) return 35;
		if (engine.Step() || engine.IsRunning()) return 36;
		if (state.updates != 2 || state.stop_logs != 3) return 37;
		engine.Shutdown();
		return state.teardown_calls_rejected && state.stop_logs == 3 ? 0 : 38;
	}
}
