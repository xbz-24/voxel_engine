#include "HudRenderer.h"

#include "Render2D.h"

#include <array>
#include <string>

namespace ve::ui
{
	namespace
	{
		struct SettingsRow
		{
			SettingsMenuOption option;
			const char* label;
		};

		constexpr std::array<SettingsRow, static_cast<std::size_t>(SettingsMenuOption::Count)> Rows{{
			{ SettingsMenuOption::RenderDistance, "Render Distance" },
			{ SettingsMenuOption::DebugOverlay, "Debug Overlay" },
			{ SettingsMenuOption::FlyMode, "Fly Mode" },
			{ SettingsMenuOption::Resume, "Resume Game" },
			{ SettingsMenuOption::Quit, "Quit" }
		}};

		/**
		 * Converts a settings option into its row index.
		 */
		std::size_t RowIndex(SettingsMenuOption option)
		{
			return static_cast<std::size_t>(option);
		}

		/**
		 * Formats the value shown on the right side of one settings row.
		 */
		std::string ValueText(const SettingsMenuState& state, SettingsMenuOption option)
		{
			switch (option)
			{
			case SettingsMenuOption::RenderDistance: return "< " + std::to_string(state.renderDistanceChunks) + " chunks >";
			case SettingsMenuOption::DebugOverlay: return state.showDebugOverlay ? "ON" : "OFF";
			case SettingsMenuOption::FlyMode: return state.isFlying ? "ON" : "OFF";
			case SettingsMenuOption::Resume: return "Enter";
			case SettingsMenuOption::Quit: return "Enter";
			case SettingsMenuOption::Count: return "";
			}
			return "";
		}
	}

	void HudRenderer::DrawSettingsMenu(const HudFrameInfo& frame)
	{
		const float width = static_cast<float>(frame.window.GetWidth());
		const float height = static_cast<float>(frame.window.GetHeight());
		ve::rendering::DrawTexturedQuad(_textures.menuBackground, 0.0f, 0.0f, width, height);

		const float panelWidth = 520.0f;
		const float buttonWidth = 430.0f;
		const float buttonHeight = 42.0f;
		const float panelX = (width - panelWidth) * 0.5f;
		const float firstRowY = (height * 0.5f) - 112.0f;
		DrawText("Settings", panelX + 160.0f, firstRowY - 54.0f, 2.2f);
		DrawText("Esc close  Up/Down select  Left/Right change", panelX + 36.0f, firstRowY - 22.0f, 1.15f);

		for (std::size_t rowIndex = 0; rowIndex < Rows.size(); rowIndex++)
		{
			const SettingsRow& row = Rows[rowIndex];
			const bool selected = row.option == frame.settingsMenu.selectedOption;
			const float x = panelX + ((panelWidth - buttonWidth) * 0.5f);
			const float y = firstRowY + (static_cast<float>(rowIndex) * 48.0f);
			const GLuint buttonTexture = selected ? _textures.menuButtonHighlighted : _textures.menuButton;
			ve::rendering::DrawTexturedQuad(buttonTexture, x, y, buttonWidth, buttonHeight);
			DrawText(row.label, x + 22.0f, y + 13.0f, 1.3f);
			DrawText(ValueText(frame.settingsMenu, row.option), x + 250.0f, y + 13.0f, 1.3f);
		}

		const float sliderX = panelX + 254.0f;
		const float sliderY = firstRowY + 10.0f;
		const float sliderWidth = 150.0f;
		const float handleX = sliderX + ((static_cast<float>(frame.settingsMenu.renderDistanceChunks - 1) / 5.0f) * (sliderWidth - 16.0f));
		ve::rendering::DrawTexturedQuad(_textures.menuSlider, sliderX, sliderY + 18.0f, sliderWidth, 12.0f);
		ve::rendering::DrawTexturedQuad(_textures.menuSliderHandle, handleX, sliderY + 10.0f, 18.0f, 28.0f);
		DrawText("Selected: " + std::to_string(RowIndex(frame.settingsMenu.selectedOption) + 1), panelX + 198.0f, firstRowY + 256.0f, 1.1f);
	}
}
