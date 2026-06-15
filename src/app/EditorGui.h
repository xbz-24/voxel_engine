#pragma once

#include "EditorDemoState.h"

class Window;

namespace ve::editor
{
	/**
	 * @brief Encapsulates the lifecycle and rendering context of Dear ImGui.
	 *
	 * This class acts as a bridge between the host windowing system (GLFW),
	 * the graphics backend (OpenGL3), and the ImGui context. It strictly manages
	 * initialization state to prevent double-initialization or dangling pointer
	 * access during shutdown.
	 */

	class EditorGui
	{
	public:
		
		/**
		 * @brief Bootstraps the ImGui context and binds hardware backends.
		 * * @param window The engine window instance that owns the active graphics context.
		 * @return True if the ImGui context and backends initialized successfully; false if
		 * the window's graphics API is unsupported or initialization fails.
		 */

		bool Initialize(Window& window);

		/** 
		 * @brief Prepares the internal ImGui state for a new frame.
		 * @note Must be called exactly once per frame, before any ImGui UI calls.
		 */

		void BeginFrame();

		/**
		 * @brief Renders the primary developer control panel.
		 * * @param state Mutable reference to the centralized editor state. The UI will
		 * read from and write directly to this struct based on user interaction.
		 */

		void DrawDemoPanel(EditorDemoState& state);

		/** 
		 * @brief Flushes ImGui's internal command buffers to the graphics API.
		 * @note Must be called exactly once per frame, after all UI calls and before swapping buffers.
		 */

		void Render();

		/** 
		 * @brief Safely tears down the ImGui context and unbinds backends.
		 * @note Safe to call multiple times; internally guarded by the initialization flag.
		 */

		void Shutdown();

		/**
		 * @brief Queries whether ImGui is actively consuming mouse input.
		 * * @return True if the mouse is hovering over or interacting with an ImGui window.
		 * The core engine should use this to drop/ignore mouse events to prevent "click-through" bugs.
		 */

		bool WantsMouseInput() const noexcept;

	private:
		bool is_initialized_ = false;
	};
}
