#pragma once

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "HudFrameInfo.h"

#include <GL/glew.h>
#include <cstddef>
#include <string>

namespace ve::ui
{
	class HudRenderer
	{
	public:
		/// Loads all HUD textures from resolved asset paths.
		explicit HudRenderer(const ve::assets::AssetPaths& paths);

		/// Draws the HUD overlay for the current frame.
		void Draw(const HudFrameInfo& frame);

	private:
		struct Textures
		{
			GLuint crosshair;
			GLuint hotbar;
			GLuint hotbar_selection;
			GLuint experience_bar;
			GLuint heart;
			GLuint hunger;
			GLuint font;
			GLuint menu_background;
			GLuint menu_button;
			GLuint menu_button_highlighted;
			GLuint menu_slider;
			GLuint menu_slider_handle;
		};

		/// Draws a screen-space row of icons.
		void DrawIconRow(GLuint texture, float row_start_x, float icon_y, float icon_size, float icon_spacing, int icon_count, float direction);

		/// Draws the fixed voxel HUD widgets.
		void DrawSurvivalHud(const ve::engine::Window& window, const ve::blocks::BlockRegistry& block_registry, ve::blocks::BlockId selected_placement_block);

		/// Draws debug text readouts.
		void DrawDebugOverlay(const HudFrameInfo& frame);

		/// Draws the settings menu overlay.
		void DrawSettingsMenu(const HudFrameInfo& frame);
		/// Draws the title and controls text for the settings menu.
		void DrawSettingsHeader(float panel_x, float first_row_y);
		/// Draws one selectable settings menu row.
		void DrawSettingsRow(
			const HudFrameInfo& frame,
			SettingsMenuOption option,
			const char* label,
			std::size_t row_index,
			float panel_x,
			float first_row_y);
		/// Draws the render-distance slider for the settings menu.
		void DrawSettingsSlider(const HudFrameInfo& frame, float panel_x, float first_row_y);

		/// Draws text using the loaded bitmap font.
		void DrawText(const std::string& text, float x, float y, float scale);

		Textures textures_;
	};
}
