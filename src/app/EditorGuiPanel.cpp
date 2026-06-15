#include "EditorGui.h"

#include "CoreTypes.h"

#include <imgui.h>
#include <imgui_impl_opengl3.h>

namespace
{
	/// Draws the demo selector combo box.
	void DrawDemoSelector(ve::editor::EditorDemoState& state)
	{
		static constexpr ve::core::StaticArray<ve::editor::DemoGame, 3> demos{ {
			ve::editor::DemoGame::MinecraftSandbox,
			ve::editor::DemoGame::VoxelApiShowcase,
			ve::editor::DemoGame::EmptyPrototype
		} };
		const char* current_name = ve::editor::DemoGameName(state.selected_demo);
		if (!ImGui::BeginCombo("Demo", current_name)) return;
		for (const ve::editor::DemoGame demo : demos)
		{
			const bool is_selected = demo == state.selected_demo;
			if (ImGui::Selectable(ve::editor::DemoGameName(demo), is_selected)) state.selected_demo = demo;
			if (is_selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

namespace ve::editor
{
	/// Draws the runtime demo panel.
	void EditorGui::DrawDemoPanel(EditorDemoState& state)
	{
		if (!is_initialized_) return;
		ImGui::Begin("Engine API Demo");
		DrawDemoSelector(state);
		ImGui::SliderInt("World size", &state.requested_world_size_chunks, 2, 24);
		ImGui::SliderInt("Render distance", &state.render_distance_chunks, 1, 10);
		ImGui::Checkbox("VSync", &state.is_vsync_enabled);
		ImGui::Checkbox("ImGui demo", &state.show_imgui_demo_window);
		if (ImGui::Button("Rebuild demo")) state.request_demo_rebuild = true;
		ImGui::End();
		if (state.show_imgui_demo_window) ImGui::ShowDemoWindow(&state.show_imgui_demo_window);
	}

	/// Presents queued ImGui draw data.
	void EditorGui::Render()
	{
		if (!is_initialized_) return;
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
