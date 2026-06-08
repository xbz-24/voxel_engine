#include "HudRenderer.h"

#include "Utilities.h"

#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ve::ui
{
	HudRenderer::HudRenderer(const ve::assets::AssetPaths& paths)
		: _textures{
			Utils::load_texture(paths.crosshairTexture.string().c_str()),
			Utils::load_texture(paths.hotbarTexture.string().c_str()),
			Utils::load_texture(paths.hotbarSelectionTexture.string().c_str()),
			Utils::load_texture(paths.experienceBarTexture.string().c_str()),
			Utils::load_texture(paths.healthTexture.string().c_str()),
			Utils::load_texture(paths.hungerTexture.string().c_str()),
			Utils::load_texture(paths.fontTexture.string().c_str())
		}
	{
	}

	void HudRenderer::Draw(const Window& window, const Camera& camera, int displayedFps, const glm::ivec3& targetBlock, bool isBlockSelected, const ve::blocks::BlockRegistry& blockRegistry, ve::blocks::BlockId selectedPlacementBlock, bool showDebugOverlay, bool isFlying, int renderDistanceChunks)
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		const glm::mat4 projection2D = glm::ortho(0.0f, static_cast<float>(window.GetWidth()), static_cast<float>(window.GetHeight()), 0.0f, -1.0f, 1.0f);
		glLoadMatrixf(glm::value_ptr(projection2D));
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor3f(1.0f, 1.0f, 1.0f);

		DrawSurvivalHud(window, blockRegistry, selectedPlacementBlock);
		DrawDebugOverlay(camera, displayedFps, targetBlock, isBlockSelected, blockRegistry, selectedPlacementBlock, showDebugOverlay, isFlying, renderDistanceChunks);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
}
