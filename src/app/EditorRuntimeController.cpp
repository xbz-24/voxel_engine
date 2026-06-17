#include "EditorRuntimeController.h"

#include "RuntimeSettings.h"
#include "Window.h"

namespace ve::editor
{
	/// Initializes the editor UI from current runtime settings.
	bool EditorRuntimeController::Initialize(ve::engine::Window& window, const ve::gameplay::RuntimeSettings& settings)
	{
		state_.render_distance_chunks = settings.renderDistanceChunks;
		state_.is_vsync_enabled = settings.isVSyncEnabled;
		return gui_.Initialize(window);
	}

	/// Starts a new editor UI frame.
	void EditorRuntimeController::BeginFrame()
	{
		gui_.BeginFrame();
	}

	/// Draws the editor and applies edited runtime settings.
	void EditorRuntimeController::DrawAndApply(ve::engine::Window& window, ve::gameplay::RuntimeSettings& settings)
	{
		gui_.DrawDemoPanel(state_);
		settings.renderDistanceChunks = state_.render_distance_chunks;
		settings.isVSyncEnabled = state_.is_vsync_enabled;
		if (window.IsVSyncEnabled() != state_.is_vsync_enabled) window.SetVSync(state_.is_vsync_enabled);
		state_.request_demo_rebuild = false;
	}

	/// Presents the editor UI.
	void EditorRuntimeController::Render()
	{
		gui_.Render();
	}

	/// Releases editor UI resources.
	void EditorRuntimeController::Shutdown()
	{
		gui_.Shutdown();
	}
}
