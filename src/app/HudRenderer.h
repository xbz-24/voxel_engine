#pragma once

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "Camera.h"
#include "SettingsMenu.h"
#include "Window.h"

#include <GL/glew.h>
#include <cstddef>
#include <string>

namespace ve::ui
{
	struct HudFrameInfo
	{
		HudFrameInfo(const ve::engine::Window& frame_window,
			const Camera& frame_camera,
			int fps,
			glm::ivec3 selected_target_block,
			bool has_selected_block,
			const ve::blocks::BlockRegistry& registry,
			ve::blocks::BlockId placement_block,
			bool should_show_debug_overlay,
			bool is_player_flying,
			int render_distance_chunks,
			std::size_t pending_events,
			SettingsMenuState settings_menu_state)
			: window(frame_window),
			  camera(frame_camera),
			  displayedFps(fps),
			  targetBlock(selected_target_block),
			  isBlockSelected(has_selected_block),
			  blockRegistry(registry),
			  selectedPlacementBlock(placement_block),
			  showDebugOverlay(should_show_debug_overlay),
			  isFlying(is_player_flying),
			  renderDistanceChunks(render_distance_chunks),
			  pendingWorldEvents(pending_events),
			  settingsMenu(settings_menu_state)
		{
		}

		const ve::engine::Window& window;
		const Camera& camera;
		int displayedFps;
		glm::ivec3 targetBlock;
		bool isBlockSelected;
		const ve::blocks::BlockRegistry& blockRegistry;
		ve::blocks::BlockId selectedPlacementBlock;
		bool showDebugOverlay;
		bool isFlying;
		int renderDistanceChunks;
		std::size_t pendingWorldEvents;
		SettingsMenuState settingsMenu;
	};

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
			GLuint hotbarSelection;
			GLuint experienceBar;
			GLuint heart;
			GLuint hunger;
			GLuint font;
			GLuint menuBackground;
			GLuint menuButton;
			GLuint menuButtonHighlighted;
			GLuint menuSlider;
			GLuint menuSliderHandle;
		};

		/// Draws a screen-space row of icons.
		void DrawIconRow(GLuint texture, float startX, float y, float iconSize, float spacing, int count, float direction);

		/// Draws the fixed voxel HUD widgets.
		void DrawSurvivalHud(const ve::engine::Window& window, const ve::blocks::BlockRegistry& blockRegistry, ve::blocks::BlockId selectedPlacementBlock);

		/// Draws debug text readouts.
		void DrawDebugOverlay(const HudFrameInfo& frame);

		/// Draws the settings menu overlay.
		void DrawSettingsMenu(const HudFrameInfo& frame);
		/// Draws the title and controls text for the settings menu.
		void DrawSettingsHeader(float panelX, float firstRowY);
		/// Draws one selectable settings menu row.
		void DrawSettingsRow(const HudFrameInfo& frame, SettingsMenuOption option, const char* label, std::size_t rowIndex, float panelX, float firstRowY);
		/// Draws the render-distance slider for the settings menu.
		void DrawSettingsSlider(const HudFrameInfo& frame, float panelX, float firstRowY);

		/// Draws text using the loaded bitmap font.
		void DrawText(const std::string& text, float x, float y, float scale);

		Textures _textures;
	};
}
