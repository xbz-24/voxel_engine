#pragma once

#include "AssetPaths.h"
#include "EditorRuntimeController.h"
#include "Engine.h"
#include "FrameTimer.h"
#include "GameController.h"
#include "GameModel.h"
#include "RuntimeInput.h"
#include "RuntimeInputRouter.h"
#include "RuntimeRenderDriver.h"
#include "RuntimeRenderHost.h"

#include <memory>

namespace ve::engine
{
	/** Owns the active runtime systems created by EngineApplication::Run. */
	class EngineRuntime : private RuntimeRenderHost
	{
	public:
		/** Keeps a reference to the engine facade that owns shared settings and callbacks. */
		explicit EngineRuntime(EngineApplication& engine) noexcept;

		/** Initializes systems, runs the frame loop, shuts down, and returns a process status. */
		[[nodiscard]] int Execute();

		/** Initializes all runtime systems without entering the main loop. */
		[[nodiscard]] EngineStartupResult Start();

		/** Runs exactly one frame and reports whether another frame should be requested. */
		[[nodiscard]] bool Step();

		/** Releases runtime systems and returns the object to an unstarted state. */
		void Shutdown();

	private:
		[[nodiscard]] EngineStartupResult Initialize();
		void PrepareAssetsAndLogging();
		[[nodiscard]] EngineStartupResult CreateRuntimeSystems();
		[[nodiscard]] EngineStartupResult CreateRenderDriver();

		void RunMainLoop();
		void RunFrame();
		[[nodiscard]] bool ShouldContinue() const noexcept;
		void BeginRuntimeFrame();
		void UpdateViewportProjection() override;
		void RenderWorld(RenderView& renderView) override;
		void RenderHud(RenderView& renderView) override;
		void EndRuntimeFrame();
		void ApplyConfiguredWorldEditsOnce();
		void ApplyWorldEdits(const std::vector<WorldBlockEdit>& edits);
		void InvokePublicApiFrameCallbacks();
		EngineApplication& engine_;
		Window window_;
		RuntimeInputRouter input_router_;
		ve::assets::AssetPaths asset_paths_;
		std::unique_ptr<GameModel> model_;
		std::unique_ptr<RuntimeRenderDriver> render_driver_;
		RuntimeInputActionTracker runtime_input_actions_;
		GameController controller_;
		ve::editor::EditorRuntimeController editor_controller_;
		ve::time::FrameTimer frame_timer_;
		double elapsed_seconds_ = 0.0;
		bool configured_world_edits_applied_ = false;
	};
}
