#include "HudRenderer.h"

#include "TextureLoader.h"

#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ve::ui
{
	HudRenderer::HudRenderer(const ve::assets::AssetPaths& paths)
		: _textures{
			ve::rendering::LoadTexture(paths.crosshairTexture.string().c_str()),
			ve::rendering::LoadTexture(paths.hotbarTexture.string().c_str()),
			ve::rendering::LoadTexture(paths.hotbarSelectionTexture.string().c_str()),
			ve::rendering::LoadTexture(paths.experienceBarTexture.string().c_str()),
			ve::rendering::LoadTexture(paths.healthTexture.string().c_str()),
			ve::rendering::LoadTexture(paths.hungerTexture.string().c_str()),
			ve::rendering::LoadTexture(paths.fontTexture.string().c_str()),
			ve::rendering::LoadTexture(paths.menuBackgroundTexture.string().c_str()),
			ve::rendering::LoadTexture(paths.menuButtonTexture.string().c_str()),
			ve::rendering::LoadTexture(paths.menuButtonHighlightedTexture.string().c_str()),
			ve::rendering::LoadTexture(paths.menuSliderTexture.string().c_str()),
			ve::rendering::LoadTexture(paths.menuSliderHandleTexture.string().c_str())
		}
	{
	}

	void HudRenderer::Draw(const HudFrameInfo& frame)
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		const glm::mat4 projection2D = glm::ortho(0.0f, static_cast<float>(frame.window.GetWidth()), static_cast<float>(frame.window.GetHeight()), 0.0f, -1.0f, 1.0f);
		glLoadMatrixf(glm::value_ptr(projection2D));
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor3f(1.0f, 1.0f, 1.0f);

		if (frame.settingsMenu.isOpen)
		{
			DrawSettingsMenu(frame);
		}
		else
		{
			DrawSurvivalHud(frame.window, frame.blockRegistry, frame.selectedPlacementBlock);
			DrawDebugOverlay(frame);
		}

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
}
