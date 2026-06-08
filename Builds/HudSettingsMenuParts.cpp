#include "HudRenderer.h"

#include "Render2D.h"

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
	void HudRenderer::DrawSettingsHeader(float panelX, float firstRowY)
	{
		DrawText("Settings", panelX + 160.0f, firstRowY - 54.0f, 2.2f);
		DrawText("Esc close  Up/Down select  Left/Right change", panelX + 36.0f, firstRowY - 22.0f, 1.15f);
	}

	/// Draws one textured row and its current value.
	void HudRenderer::DrawSettingsRow(const HudFrameInfo& frame, SettingsMenuOption option, const char* label, std::size_t rowIndex, float panelX, float firstRowY)
	{
		constexpr float panelWidth = 520.0f;
		constexpr float buttonWidth = 430.0f;
		constexpr float buttonHeight = 42.0f;
		const bool selected = option == frame.settingsMenu.selectedOption;
		const float x = panelX + ((panelWidth - buttonWidth) * 0.5f);
		const float y = firstRowY + (static_cast<float>(rowIndex) * 48.0f);
		const GLuint buttonTexture = selected ? _textures.menuButtonHighlighted : _textures.menuButton;
		ve::rendering::DrawTexturedQuad(buttonTexture, x, y, buttonWidth, buttonHeight);
		DrawText(label, x + 22.0f, y + 13.0f, 1.3f);
		DrawText(ValueText(frame.settingsMenu, option), x + 250.0f, y + 13.0f, 1.3f);
	}

	/// Draws the slider mapped to the current render distance.
	void HudRenderer::DrawSettingsSlider(const HudFrameInfo& frame, float panelX, float firstRowY)
	{
		const float sliderX = panelX + 254.0f;
		const float sliderY = firstRowY + 10.0f;
		const float sliderWidth = 150.0f;
		const float handleX = sliderX + ((static_cast<float>(frame.settingsMenu.renderDistanceChunks - 1) / 5.0f) * (sliderWidth - 16.0f));
		ve::rendering::DrawTexturedQuad(_textures.menuSlider, sliderX, sliderY + 18.0f, sliderWidth, 12.0f);
		ve::rendering::DrawTexturedQuad(_textures.menuSliderHandle, handleX, sliderY + 10.0f, 18.0f, 28.0f);
	}
}
