#pragma once

#include "AssetPaths.h"
#include "EditorRuntimeController.h"
#include "Engine.h"
#include "FrameTimer.h"
#include "GameController.h"
#include "GameModel.h"
#include "RenderBackend.h"
#include "RenderView.h"
#include "VulkanFrameRenderer.h"

#include <memory>

namespace ve::engine
{
	class OpenGLRenderView;

	/** Owns the active runtime systems created by Engine::Run. */
	class EngineRuntime
	{
	public:
		/** Keeps a reference to the engine facade that owns shared settings and callbacks. */
		explicit EngineRuntime(Engine& engine) noexcept;

		/** Initializes systems, runs the frame loop, shuts down, and returns a process status. */
		[[nodiscard]] int Execute();

	private:
		[[nodiscard]] bool Initialize();
		void PrepareAssetsAndLogging();
		[[nodiscard]] bool CreateRuntimeSystems();
		[[nodiscard]] bool CreateRenderBackend();

		/** Returns the legacy view used by systems that still depend on OpenGL state. */
		[[nodiscard]] OpenGLRenderView& LegacyOpenGLView() noexcept;

		void RunMainLoop();
		void RunFrame();
		void RunOpenGLFrame();
		void RunVulkanFrame();
		void BeginRuntimeFrame();
		void UpdateGameplay(OpenGLRenderView& legacyView);
		void RenderWorld(OpenGLRenderView& legacyView);
		void RenderHud(OpenGLRenderView& legacyView);
		void EndRuntimeFrame();
		void Shutdown();

		Engine& engine_;
		Window window_{ "Voxel Engine v1.0.0" };
		Engine::CallbackContext callback_context_{ nullptr, nullptr, { 0.0, 0.0, true } };
		ve::assets::AssetPaths asset_paths_;
		std::unique_ptr<GameModel> model_;
		std::unique_ptr<ve::rendering::RenderBackend> backend_;
		std::unique_ptr<RenderView> view_;
		ve::rendering::VulkanFrameRenderer vulkan_frame_renderer_;
		GameController controller_;
		ve::editor::EditorRuntimeController editor_controller_;
		ve::time::FrameTimer frame_timer_;
	};
}
