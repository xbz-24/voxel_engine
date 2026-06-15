#include "HudRenderer.h"

#include "Render2D.h"

#include <array>

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
			{ SettingsMenuOption::VSync, "VSync" },
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
	}

	/// Draws the full settings menu overlay.
	void HudRenderer::DrawSettingsMenu(const HudFrameInfo& frame)
	{
		const float width = static_cast<float>(frame.window.GetWidth());
		const float height = static_cast<float>(frame.window.GetHeight());
		ve::rendering::DrawTexturedQuad(_textures.menuBackground, 0.0f, 0.0f, width, height);

		const float panelWidth = 520.0f;
		const float panelX = (width - panelWidth) * 0.5f;
		const float firstRowY = (height * 0.5f) - 112.0f;
		DrawSettingsHeader(panelX, firstRowY);

		for (std::size_t rowIndex = 0; rowIndex < Rows.size(); rowIndex++)
		{
			const SettingsRow& row = Rows[rowIndex];
			DrawSettingsRow(frame, row.option, row.label, rowIndex, panelX, firstRowY);
		}

		DrawSettingsSlider(frame, panelX, firstRowY);
		DrawText("Selected: " + std::to_string(RowIndex(frame.settingsMenu.selectedOption) + 1), panelX + 198.0f, firstRowY + 304.0f, 1.1f);
	}
}
