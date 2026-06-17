#pragma once

#include "EditorDemoState.h"
#include "EditorGui.h"

namespace ve::engine { class Window; }

namespace ve::gameplay
{
	struct RuntimeSettings;
}

namespace ve::editor
{
	class EditorRuntimeController
	{
	public:
		/** @param window Engine window used by ImGui. @param settings Runtime settings copied into editor state. @return True when initialized. */
		bool Initialize(ve::engine::Window& window, const ve::gameplay::RuntimeSettings& settings);

		/** Starts a new editor UI frame. */
		void BeginFrame();

		/** @param window Runtime window updated by editor settings. @param settings Runtime settings modified by editor controls. */
		void DrawAndApply(ve::engine::Window& window, ve::gameplay::RuntimeSettings& settings);

		/** Presents the editor UI. */
		void Render();

		/** Releases editor UI resources. */
		void Shutdown();

	private:
		EditorGui gui_;
		EditorDemoState state_;
	};
}
