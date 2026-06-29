#include "HudRenderer.h"

#include "TextureLoader.h"

#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>

namespace ve::ui
{
	namespace
	{
		GLuint LoadHudTexture(const std::filesystem::path& path)
		{
			// TODO: Load HUD textures through backend-owned texture resources instead of forcing OpenGL ids at construction.
			return ve::rendering::NativeOpenGLTexture(ve::rendering::LoadTexture(path.string().c_str()));
		}
	}

	HudRenderer::HudRenderer(const ve::assets::AssetPaths& paths)
		: textures_{
			LoadHudTexture(paths.crosshairTexture),
			LoadHudTexture(paths.hotbarTexture),
			LoadHudTexture(paths.hotbarSelectionTexture),
			LoadHudTexture(paths.experienceBarTexture),
			LoadHudTexture(paths.healthTexture),
			LoadHudTexture(paths.hungerTexture),
			LoadHudTexture(paths.fontTexture),
			LoadHudTexture(paths.menuBackgroundTexture),
			LoadHudTexture(paths.menuButtonTexture),
			LoadHudTexture(paths.menuButtonHighlightedTexture),
			LoadHudTexture(paths.menuSliderTexture),
			LoadHudTexture(paths.menuSliderHandleTexture)
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

		if (frame.settings_menu.isOpen)
		{
			DrawSettingsMenu(frame);
		}
		else
		{
			DrawSurvivalHud(frame.window, frame.block_registry, frame.selected_placement_block);
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
