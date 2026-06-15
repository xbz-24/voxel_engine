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

	/**
	 * @brief Manages the runtime systems created by Engine::Run and orchestrates the main loop.
	 * @details This class isolates the execution lifecycle from the main \c Engine class.
	 * The purpose of this separation is to maintain the readability of the frame loop
	 * and prevent the base configuration object (\c Engine) from directly controlling the active window resources,
	 * the game model, and the graphics pipeline. It transiently handles the compatibility between OpenGL and Vulkan.
	 */

	class EngineRuntime
	{
	public:
		
		/** 
		 * @brief Initializes the runtime environment by linking it to the main engine instance.
		 * @param engine Main engine facade that owns shared settings and callbacks.
		 * @details Injecting the \c Engine dependency is required so that runtime controllers
		 * can dispatch events or read global settings without resorting to global variables (Singletons).
		
		*/
	
		explicit EngineRuntime(Engine& engine) noexcept;

		/** 
		* @brief Executes the complete lifecycle: initialization, main loop, and shutdown.
    	* @return Process-style status code (e.g., 0 for success, non-zero for error) after running or failing startup.
		* @details Acts as the main entry point for the calling thread. If systems fail to initialize,
		* it safely aborts before entering the infinite loop, preventing segmentation faults.
		*/
		
		[[nodiscard]] int Execute();

	private:
		/** 
		* @brief Orchestrates the sequential startup of all required subsystems.
		* @return \c true if all runtime systems are ready; \c false if a critical error occurred.
		*/
		[[nodiscard]] bool Initialize();

		/** Resolves assets and configures runtime log sinks. */
		void PrepareAssetsAndLogging();

		/** @return True when model, callbacks, editor UI, and view resources are ready. */
		[[nodiscard]] bool CreateRuntimeSystems();

		/** @return True when the selected graphics backend is initialized. */
		[[nodiscard]] bool CreateRenderBackend();

		/** @return Current OpenGL compatibility view required by the legacy renderer. */
		[[nodiscard]] OpenGLRenderView& LegacyOpenGLView() noexcept;

		/** Runs frames until the window requests shutdown. */
		void RunMainLoop();

		/** Runs one complete gameplay, render, UI, and present frame. */
		void RunFrame();

		/** Runs the existing voxel renderer through the compatibility backend. */
		void RunOpenGLFrame();

		/** Clears and presents one frame through the Vulkan backend. */
		void RunVulkanFrame();

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
		std::unique_ptr<ve::rendering::RenderBackend> backend_;
		std::unique_ptr<RenderView> view_;
		ve::rendering::VulkanFrameRenderer vulkan_frame_renderer_;
		GameController controller_;
		ve::editor::EditorRuntimeController editor_controller_;
		ve::time::FrameTimer frame_timer_;
	};
}
