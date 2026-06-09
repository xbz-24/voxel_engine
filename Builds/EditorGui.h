#pragma once

#include "EditorDemoState.h"

class Window;

namespace ve::editor
{
	class EditorGui
	{
	public:
		/** @param window Engine window that owns the OpenGL context. @return True when ImGui starts. */
		bool Initialize(Window& window);

		/** Starts a new ImGui frame. */
		void BeginFrame();

		/** @param state Mutable editor/demo state exposed to controls. */
		void DrawDemoPanel(EditorDemoState& state);

		/** Presents queued ImGui draw data. */
		void Render();

		/** Releases ImGui backends and context. */
		void Shutdown();

		/** @return True when ImGui owns current mouse input. */
		bool WantsMouseInput() const noexcept;

	private:
		bool is_initialized_ = false;
	};
}
