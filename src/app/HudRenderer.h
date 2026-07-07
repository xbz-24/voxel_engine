#pragma once

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "HudFrameInfo.h"
#include "GraphicsTypes.h"

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
			ve::rendering::TextureHandle crosshair;
			ve::rendering::TextureHandle hotbar;
			ve::rendering::TextureHandle hotbar_selection;
			ve::rendering::TextureHandle experience_bar;
			ve::rendering::TextureHandle heart;
			ve::rendering::TextureHandle hunger;
			ve::rendering::TextureHandle font;
			ve::rendering::TextureHandle menu_background;
			ve::rendering::TextureHandle menu_button;
			ve::rendering::TextureHandle menu_button_highlighted;
			ve::rendering::TextureHandle menu_slider;
			ve::rendering::TextureHandle menu_slider_handle;
		};

		/// Draws a screen-space row of icons.
		void DrawIconRow(ve::rendering::TextureHandle texture, float row_start_x, float icon_y, float icon_size, float icon_spacing, int icon_count, float direction);

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
