#include "HudRenderer.h"

#include "Render2D.h"
#include "RenderDistanceSettings.h"

#include <string>

namespace ve::ui
{
	namespace
	{
		/// Formats the value shown on the right side of one settings row.
		std::string ValueText(const SettingsMenuState& state, SettingsMenuOption option)
		{
			switch (option)
			{
			case SettingsMenuOption::RenderDistance: return "< " + std::to_string(state.renderDistanceChunks) + " chunks >";
			case SettingsMenuOption::VSync: return state.isVSyncEnabled ? "ON" : "OFF";
			case SettingsMenuOption::DebugOverlay: return state.showDebugOverlay ? "ON" : "OFF";
			case SettingsMenuOption::FlyMode: return state.isFlying ? "ON" : "OFF";
			case SettingsMenuOption::Resume: return "Enter";
			case SettingsMenuOption::Quit: return "Enter";
			case SettingsMenuOption::Count: return "";
			}
			return "";
		}
	}

	/// Draws the title and keyboard help for the settings menu.
	void HudRenderer::DrawSettingsHeader(float panel_x, float first_row_y)
	{
		DrawText("Settings", panel_x + 160.0f, first_row_y - 54.0f, 2.2f);
		DrawText("Esc close  Up/Down select  Left/Right change", panel_x + 36.0f, first_row_y - 22.0f, 1.15f);
	}

	/// Draws one textured row and its current value.
	void HudRenderer::DrawSettingsRow(
		const HudFrameInfo& frame,
		SettingsMenuOption option,
		const char* label,
		std::size_t row_index,
		float panel_x,
		float first_row_y)
	{
		constexpr float kPanelWidth = 520.0f;
		constexpr float kButtonWidth = 430.0f;
		constexpr float kButtonHeight = 42.0f;
		const bool is_selected = option == frame.settings_menu.selectedOption;
		const float row_x = panel_x + ((kPanelWidth - kButtonWidth) * 0.5f);
		const float row_y = first_row_y + (static_cast<float>(row_index) * 48.0f);
		const GLuint button_texture =
			is_selected ? textures_.menu_button_highlighted : textures_.menu_button;
		ve::rendering::DrawTexturedQuad(button_texture, row_x, row_y, kButtonWidth, kButtonHeight);
		DrawText(label, row_x + 22.0f, row_y + 13.0f, 1.3f);
		DrawText(ValueText(frame.settings_menu, option), row_x + 250.0f, row_y + 13.0f, 1.3f);
	}

	/// Draws the slider mapped to the current render distance.
	void HudRenderer::DrawSettingsSlider(const HudFrameInfo& frame, float panel_x, float first_row_y)
	{
		const float slider_x = panel_x + 254.0f;
		const float slider_y = first_row_y + 10.0f;
		const float slider_width = 150.0f;
		const float handle_ratio =
			ve::gameplay::RenderDistanceSliderRatio(frame.settings_menu.renderDistanceChunks);
		const float handle_x = slider_x + (handle_ratio * (slider_width - 16.0f));
		ve::rendering::DrawTexturedQuad(textures_.menu_slider, slider_x, slider_y + 18.0f, slider_width, 12.0f);
		ve::rendering::DrawTexturedQuad(textures_.menu_slider_handle, handle_x, slider_y + 10.0f, 18.0f, 28.0f);
	}
}
