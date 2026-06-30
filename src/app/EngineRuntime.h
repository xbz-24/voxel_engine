#pragma once

#include "AssetPaths.h"
#include "EditorRuntimeController.h"
#include "Engine.h"
#include "FrameTimer.h"
#include "GameController.h"
#include "GameModel.h"
#include "RenderBackend.h"
#include "RenderView.h"
#include "RuntimeInput.h"
#include "VulkanFrameRenderer.h"

#include <memory>

namespace ve::engine
{
	class OpenGLRenderView;

	/** Owns the active runtime systems created by EngineApplication::Run. */
	class EngineRuntime
	{
	public:
		// TODO: Add a non-owning embedding mode where a host app calls Tick() and Present() itself.
		/** Keeps a reference to the engine facade that owns shared settings and callbacks. */
		explicit EngineRuntime(EngineApplication& engine) noexcept;

		/** Initializes systems, runs the frame loop, shuts down, and returns a process status. */
		[[nodiscard]] int Execute();

	private:
		[[nodiscard]] bool Initialize();
		void PrepareAssetsAndLogging();
		[[nodiscard]] bool CreateRuntimeSystems();
		[[nodiscard]] bool CreateRenderBackend();

		[[nodiscard]] OpenGLRenderView& LegacyOpenGLView() noexcept;

		void RunMainLoop();
		void RunFrame();
		void RunOpenGLFrame();
		void RunVulkanFrame();
		[[nodiscard]] ve::rendering::VulkanDemoInput CaptureVulkanDemoInput();
		[[nodiscard]] bool DrawVulkanFrame(const ve::rendering::VulkanDemoInput& input);
		void BeginRuntimeFrame();
		void UpdateGameplay();
		void RenderWorld(RenderView& renderView);
		void RenderHud(RenderView& renderView);
		void EndRuntimeFrame();
		void ApplyConfiguredWorldEditsOnce();
		void ApplyWorldEdits(const std::vector<WorldBlockEdit>& edits);
		void InvokePublicApiFrameCallbacks();
		void Shutdown();

		EngineApplication& engine_;
		Window window_;
		EngineApplication::CallbackContext callback_context_{ nullptr, nullptr, { 0.0, 0.0, true } };
		ve::assets::AssetPaths asset_paths_;
		std::unique_ptr<GameModel> model_;
		std::unique_ptr<ve::rendering::RenderBackend> backend_;
		std::unique_ptr<RenderView> view_;
		ve::rendering::VulkanFrameRenderer vulkan_frame_renderer_;
		// TODO: Move demo settings out of core runtime once public scenes drive Vulkan rendering.
		ve::rendering::VulkanMinecraftDemoSettings vulkan_demo_settings_;
		RuntimeInputActionTracker runtime_input_actions_;
		GameController controller_;
		ve::editor::EditorRuntimeController editor_controller_;
		ve::time::FrameTimer frame_timer_;
		double elapsed_seconds_ = 0.0;
		bool configured_world_edits_applied_ = false;
	};
}
