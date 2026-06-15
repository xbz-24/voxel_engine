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

		/** 
		* @brief Resolves asset paths and configures log event sinks.
		* @details Executed before graphics backend creation to ensure any failure
		* in loading shaders or textures can be properly written to the log files.
		*/
		void PrepareAssetsAndLogging();

		/** 
		* @brief Instantiates model logic, callbacks, editor UI, and view resources.
		* @return \c true if the model and controllers were successfully built.
		*/
		[[nodiscard]] bool CreateRuntimeSystems();

		/** 
		* @brief Initializes the underlying graphics API based on the active configuration.
		* @return \c true when the selected graphics backend has been successfully initialized.
		* @details Abstracts context initialization to support the multi-backend architecture
		* (currently bridging the legacy voxel renderer and the new Vulkan implementation).
		*/
		[[nodiscard]] bool CreateRenderBackend();

		/** 
		* @brief Retrieves the OpenGL compatibility view.
		* @brief Retrieves the OpenGL compatibility view.
		* @details This method is temporarily necessary because the original voxel engine requires 
		* a direct OpenGL context. It encapsulates a dynamic/static cast to maintain type safety.
		*/
		[[nodiscard]] OpenGLRenderView& LegacyOpenGLView() noexcept;

		/** 
		* @brief Keeps the main thread trapped processing frames until a shutdown is requested.
		* @details Continuously polls the OS-managed window event state to determine 
		* when to break the loop and proceed to \c Shutdown.
		*/
		void RunMainLoop();

		/** 
		* @brief Executes one complete frame: gameplay logic, rendering, UI, and screen presentation.
		* @details Internally determines whether to route graphics work to the OpenGL compatibility path 
		* or the native Vulkan pipeline.
		*/
		void RunFrame();

		/** 
		* @brief Runs the existing voxel renderer through the compatibility backend.
		*/
		void RunOpenGLFrame();

		/** 
		* @brief Clears buffers, issues draw commands, and presents the frame using the Vulkan backend.
		*/
		void RunVulkanFrame();

		/** 
		* @brief Begins the frame state by calculating time deltas and consuming inputs.
		* @details Separating the beginning (Begin) from the end (End) allows timing metrics, like \c FrameTimer, 
		* to provide stable deltas (\c dt) throughout the entire logic update phase, preventing asynchronous desyncs.
		*/
		void BeginRuntimeFrame();

		/** 
		* @brief Updates the internal logic of the world and entities.
		* @param legacy_view OpenGL view used by the current simulation and collision systems.
		*/
		void UpdateGameplay(OpenGLRenderView& legacy_view);

		/** 
		* @brief Sends the 3D world's meshes, matrices, and materials to the GPU.
		* @param legacy_view OpenGL view onto which the renderer projects the world.
		*/
		void RenderWorld(OpenGLRenderView& legacy_view);

		/** 
		* @brief Renders the user interface in screen space (2D).
		* @param legacy_view OpenGL view that controls the drawing state for the HUD.
		* @details Drawn after the world to ensure the Z-buffer or painter's algorithm overlays the UI correctly.
		*/
		void RenderHud(OpenGLRenderView& legacy_view);

		/** 
		* @brief Draws the editor UI and performs the buffer swap.
		* @details This phase blocks the thread if V-Sync is enabled and instructs the native window to present the back buffer.
		*/
		void EndRuntimeFrame();

		/**
		* @brief Releases rendering resources and memory structures in reverse order of creation.
		* @details Guarantees the GPU has finished all pending operations before destroying descriptors, 
		* preventing validation warnings and VRAM memory leaks.
		*/
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
