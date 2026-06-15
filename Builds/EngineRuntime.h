#pragma once

#include "AssetPaths.h"
#include "EditorRuntimeController.h"
#include "Engine.h"
#include "FrameTimer.h"
#include "GameController.h"
#include "GameModel.h"
#include "RenderView.h"

#include <memory>

namespace ve::engine
{
	class OpenGLRenderView;

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

		/** @return True when model, callbacks, editor UI, and view resources are ready. */
		[[nodiscard]] bool CreateRuntimeSystems();

		/** @return Current OpenGL compatibility view required by the legacy renderer. */
		[[nodiscard]] OpenGLRenderView& LegacyOpenGLView() noexcept;

		/** Runs frames until the window requests shutdown. */
		void RunMainLoop();

		/** Runs one complete gameplay, render, UI, and present frame. */
		void RunFrame();

		/** Updates projection, frame timer, and editor frame state. */
		void BeginRuntimeFrame();

		/** @param legacy_view OpenGL view used by the current gameplay systems. */
		void UpdateGameplay(OpenGLRenderView& legacy_view);

		/** @param legacy_view OpenGL view used by the current world renderer. */
		void RenderWorld(OpenGLRenderView& legacy_view);

		/** @param legacy_view OpenGL view used by the current HUD renderer. */
		void RenderHud(OpenGLRenderView& legacy_view);

		/** Draws editor UI and presents the native window. */
		void EndRuntimeFrame();

		/** Releases editor and engine-owned render resources. */
		void Shutdown();

		Engine& engine_;
		Window window_{ "Voxel Engine v1.0.0" };
		Engine::CallbackContext callback_context_{ nullptr, nullptr, { 0.0, 0.0, true } };
		ve::assets::AssetPaths asset_paths_;
		std::unique_ptr<GameModel> model_;
		std::unique_ptr<RenderView> view_;
		GameController controller_;
		ve::editor::EditorRuntimeController editor_controller_;
		ve::time::FrameTimer frame_timer_;
	};
}
