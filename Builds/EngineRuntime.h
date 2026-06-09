#pragma once

#include "AssetPaths.h"
#include "EditorRuntimeController.h"
#include "Engine.h"
#include "FrameTimer.h"
#include "GameController.h"
#include "GameModel.h"
#include "GameView.h"

#include <memory>

namespace ve::engine
{
	/** Owns runtime systems created by Engine::Run and keeps the frame loop readable. */
	class EngineRuntime
	{
	public:
		/** @param engine Engine facade that owns shared settings and callbacks. */
		explicit EngineRuntime(Engine& engine) noexcept;

		/** @return Process-style status code after running or failing startup. */
		[[nodiscard]] int Execute();

	private:
		/** @return True when all runtime systems are ready. */
		[[nodiscard]] bool Initialize();

		/** Resolves assets and configures runtime log sinks. */
		void PrepareAssetsAndLogging();

		/** Creates model, camera callbacks, editor UI, and view resources. */
		void CreateRuntimeSystems();

		/** Runs frames until the window requests shutdown. */
		void RunMainLoop();

		/** Runs one complete gameplay, render, UI, and present frame. */
		void RunFrame();

		/** Releases editor and engine-owned render resources. */
		void Shutdown();

		Engine& engine_;
		Window window_{ "Voxel Engine v1.0.0" };
		Engine::CallbackContext callback_context_{ nullptr, nullptr, { 0.0, 0.0, true } };
		ve::assets::AssetPaths asset_paths_;
		std::unique_ptr<GameModel> model_;
		std::unique_ptr<GameView> view_;
		GameController controller_;
		ve::editor::EditorRuntimeController editor_controller_;
		ve::time::FrameTimer frame_timer_;
	};
}
