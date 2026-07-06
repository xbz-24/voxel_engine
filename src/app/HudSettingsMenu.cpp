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
		ve::rendering::DrawTexturedQuad(textures_.menu_background, 0.0f, 0.0f, width, height);

		const float panel_width = 520.0f;
		const float panel_x = (width - panel_width) * 0.5f;
		const float first_row_y = (height * 0.5f) - 112.0f;
		DrawSettingsHeader(panel_x, first_row_y);

		for (std::size_t row_index = 0; row_index < Rows.size(); ++row_index)
		{
			const SettingsRow& row = Rows[row_index];
			DrawSettingsRow(frame, row.option, row.label, row_index, panel_x, first_row_y);
		}

		DrawSettingsSlider(frame, panel_x, first_row_y);
		DrawText("Selected: " + std::to_string(RowIndex(frame.settings_menu.selected_option) + 1), panel_x + 198.0f, first_row_y + 304.0f, 1.1f);
	}
}
