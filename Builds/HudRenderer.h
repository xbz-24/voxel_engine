#pragma once

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "Camera.h"
#include "Window.h"

#include <GL/glew.h>
#include <string>

namespace ve::ui
{
	/**
	 * Owns texture ids and draw logic for the in-game HUD.
	 */
	class HudRenderer
	{
	public:
		/**
		 * Loads all HUD textures from resolved asset paths.
		 *
		 * @param paths Resolved engine asset paths.
		 */
		explicit HudRenderer(const ve::assets::AssetPaths& paths);

		/**
		 * Draws the HUD overlay for the current frame.
		 *
		 * @param window Window used for screen-space dimensions.
		 * @param camera Camera used by debug readouts.
		 * @param displayedFps Last sampled FPS value.
		 * @param targetBlock Currently selected block coordinate.
		 * @param isBlockSelected Whether selectedBlock is valid.
		 * @param blockRegistry Registry used to display block names.
		 * @param selectedPlacementBlock Block currently selected for placement.
		 * @param showDebugOverlay Whether debug text should be visible.
		 * @param isFlying Whether player movement is currently in fly mode.
		 */
		void Draw(const Window& window, const Camera& camera, int displayedFps, const glm::ivec3& targetBlock, bool isBlockSelected, const ve::blocks::BlockRegistry& blockRegistry, ve::blocks::BlockId selectedPlacementBlock, bool showDebugOverlay, bool isFlying);

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
		};

		/**
		 * Draws a screen-space row of icons.
		 *
		 * @param texture OpenGL texture id used by each icon.
		 * @param startX X position of the first icon.
		 * @param y Y position shared by every icon.
		 * @param iconSize Icon width and height in pixels.
		 * @param spacing Distance in pixels between icon origins.
		 * @param count Number of icons to draw.
		 * @param direction Horizontal direction, usually 1.0f or -1.0f.
		 */
		void DrawIconRow(GLuint texture, float startX, float y, float iconSize, float spacing, int count, float direction);

		/**
		 * Draws the fixed Minecraft-like HUD widgets.
		 *
		 * @param window Window used for screen-space positioning.
		 * @param blockRegistry Registry used to draw block icons.
		 * @param selectedPlacementBlock Currently selected hotbar block.
		 */
		void DrawSurvivalHud(const Window& window, const ve::blocks::BlockRegistry& blockRegistry, ve::blocks::BlockId selectedPlacementBlock);

		/**
		 * Draws debug text readouts.
		 *
		 * @param camera Camera used by position readouts.
		 * @param displayedFps Last sampled FPS value.
		 * @param targetBlock Currently selected block coordinate.
		 * @param isBlockSelected Whether selectedBlock is valid.
		 * @param blockRegistry Registry used to display block names.
		 * @param selectedPlacementBlock Block currently selected for placement.
		 * @param showDebugOverlay Whether debug text should be visible.
		 * @param isFlying Whether player movement is currently in fly mode.
		 */
		void DrawDebugOverlay(const Camera& camera, int displayedFps, const glm::ivec3& targetBlock, bool isBlockSelected, const ve::blocks::BlockRegistry& blockRegistry, ve::blocks::BlockId selectedPlacementBlock, bool showDebugOverlay, bool isFlying);

		/**
		 * Draws text using the loaded bitmap font.
		 *
		 * @param text Text to draw.
		 * @param x Left screen coordinate.
		 * @param y Top screen coordinate.
		 * @param scale Glyph scale multiplier.
		 */
		void DrawText(const std::string& text, float x, float y, float scale);

		Textures _textures;
	};
}
