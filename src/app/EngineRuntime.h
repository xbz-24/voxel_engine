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
#include <bitset>

namespace ve::engine
{
	class OpenGLRenderView;

	class VulkanInputController {
	public:
		// Las acciones disponibles son públicas, para que EngineRuntime pueda pedirlas.
		enum class Action : size_t { LeftClick, F1, F2, Count };

		// Actualiza el estado leyendo directamente de la ventana.
		void Update(GLFWwindow* window);

		// Métodos de consulta limpios y semánticos.
		bool IsDown(Action action) const;
		bool IsJustPressed(Action action) const;

	private:
		// El estado interno queda completamente oculto (Information Hiding).
		std::bitset<static_cast<size_t>(Action::Count)> current_state_;
		std::bitset<static_cast<size_t>(Action::Count)> previous_state_;
	};

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

		VulkanInputController vulkan_input_;

		[[nodiscard]] OpenGLRenderView& LegacyOpenGLView() noexcept;

		void RunMainLoop();
		void RunFrame();
		void RunOpenGLFrame();
		void RunVulkanFrame();
		void BeginRuntimeFrame();
		void UpdateGameplay();
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
		ve::rendering::VulkanMinecraftDemoSettings vulkan_demo_settings_;
		GameController controller_;
		ve::editor::EditorRuntimeController editor_controller_;
		ve::time::FrameTimer frame_timer_;
	};
}
