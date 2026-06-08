#include "HudRenderer.h"

#include "Render2D.h"
#include "Utilities.h"

#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iomanip>
#include <sstream>

namespace
{
	/**
	 * Formats a vector with one decimal place for debug UI.
	 *
	 * @param value Vector to format.
	 * @return Human-readable vector string.
	 */
	std::string FormatVec3(const glm::vec3& value)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(1)
			<< value.x << ", " << value.y << ", " << value.z;
		return stream.str();
	}

	/**
	 * Formats an integer vector for debug UI.
	 *
	 * @param value Vector to format.
	 * @return Human-readable vector string.
	 */
	std::string FormatIvec3(const glm::ivec3& value)
	{
		std::ostringstream stream;
		stream << value.x << ", " << value.y << ", " << value.z;
		return stream.str();
	}
}

namespace ve::ui
{
	HudRenderer::HudRenderer(const ve::assets::AssetPaths& paths)
		: _textures{
			Utils::load_texture(paths.crosshairTexture.string().c_str()),
			Utils::load_texture(paths.hotbarTexture.string().c_str()),
			Utils::load_texture(paths.experienceBarTexture.string().c_str()),
			Utils::load_texture(paths.healthTexture.string().c_str()),
			Utils::load_texture(paths.hungerTexture.string().c_str()),
			Utils::load_texture(paths.fontTexture.string().c_str())
		}
	{
	}

	void HudRenderer::Draw(const Window& window, const Camera& camera, int displayedFps, const glm::ivec3& selectedBlock, bool isBlockSelected)
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

		DrawSurvivalHud(window);
		DrawDebugOverlay(camera, displayedFps, selectedBlock, isBlockSelected);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	void HudRenderer::DrawIconRow(GLuint texture, float startX, float y, float iconSize, float spacing, int count, float direction)
	{
		for (int i = 0; i < count; i++)
		{
			const float iconX = startX + (static_cast<float>(i) * spacing * direction);
			ve::rendering::DrawTexturedQuad(texture, iconX, y, iconSize, iconSize);
		}
	}

	void HudRenderer::DrawSurvivalHud(const Window& window)
	{
		const float screenCenterViewportX = window.GetWidth() / 2.0f;
		const float screenCenterViewportY = window.GetHeight() / 2.0f;
		const float crosshairHalfSizePixels = 30.0f;

		ve::rendering::DrawTexturedQuad(
			_textures.crosshair,
			screenCenterViewportX - crosshairHalfSizePixels,
			screenCenterViewportY - crosshairHalfSizePixels,
			crosshairHalfSizePixels * 2.0f,
			crosshairHalfSizePixels * 2.0f);

		const float globalGuiScalingFactor = 3.0f;
		const float hotbarRenderWidth = 182.0f * globalGuiScalingFactor;
		const float hotbarRenderHeight = 22.0f * globalGuiScalingFactor;
		const float hotbarScreenPositionX = (window.GetWidth() / 2.0f) - (hotbarRenderWidth / 2.0f);
		const float hotbarScreenPositionY = window.GetHeight() - hotbarRenderHeight;

		ve::rendering::DrawTexturedQuad(_textures.hotbar, hotbarScreenPositionX, hotbarScreenPositionY, hotbarRenderWidth, hotbarRenderHeight);

		const float experienceBarRenderWidth = 182.0f * globalGuiScalingFactor;
		const float experienceBarRenderHeight = 5.0f * globalGuiScalingFactor;
		const float experienceBarAnchorScreenPosX = (window.GetWidth() / 2.0f) - (experienceBarRenderWidth / 2.0f);
		const float experienceBarAnchorScreenPosY = hotbarScreenPositionY - experienceBarRenderHeight - (2.0f * globalGuiScalingFactor);

		ve::rendering::DrawTexturedQuad(_textures.experienceBar, experienceBarAnchorScreenPosX, experienceBarAnchorScreenPosY, experienceBarRenderWidth, experienceBarRenderHeight);

		const float iconSize = 9.0f * globalGuiScalingFactor;
		const float guiIconHorizontalSpacingOffset = 8.0f * globalGuiScalingFactor;
		const float iconsStartY = experienceBarAnchorScreenPosY - iconSize - (1.0f * globalGuiScalingFactor);

		DrawIconRow(_textures.heart, experienceBarAnchorScreenPosX, iconsStartY, iconSize, guiIconHorizontalSpacingOffset, 10, 1.0f);
		DrawIconRow(_textures.hunger, experienceBarAnchorScreenPosX + experienceBarRenderWidth - iconSize, iconsStartY, iconSize, guiIconHorizontalSpacingOffset, 10, -1.0f);
	}

	void HudRenderer::DrawDebugOverlay(const Camera& camera, int displayedFps, const glm::ivec3& selectedBlock, bool isBlockSelected)
	{
		const glm::vec3 cameraPosition = camera.GetPosition();
		DrawText(std::to_string(displayedFps) + " FPS", 10.0f, 10.0f, 2.0f);
		DrawText("XYZ " + FormatVec3(cameraPosition), 10.0f, 32.0f, 1.4f);

		const std::string selectedText = isBlockSelected ? "Target " + FormatIvec3(selectedBlock) : "Target none";
		DrawText(selectedText, 10.0f, 50.0f, 1.4f);
	}

	void HudRenderer::DrawText(const std::string& text, float x, float y, float scale)
	{
		glBindTexture(GL_TEXTURE_2D, _textures.font);
		glBegin(GL_QUADS);

		const float charSize = 10.0f * scale;
		const float uvStep = 1.0f / 16.0f;
		float currentCursorX = x;

		for (char c : text)
		{
			if (c == ' ')
			{
				currentCursorX += charSize * 0.8f;
				continue;
			}

			const int asciiCode = static_cast<int>(c);
			const int col = asciiCode % 16;
			const int row = 15 - (asciiCode / 16);
			const float uvX = col * uvStep;
			const float uvY = row * uvStep;

			glTexCoord2f(uvX, uvY + uvStep); glVertex2f(currentCursorX, y);
			glTexCoord2f(uvX + uvStep, uvY + uvStep); glVertex2f(currentCursorX + charSize, y);
			glTexCoord2f(uvX + uvStep, uvY); glVertex2f(currentCursorX + charSize, y + charSize);
			glTexCoord2f(uvX, uvY); glVertex2f(currentCursorX, y + charSize);

			currentCursorX += charSize * 0.8f;
		}

		glEnd();
	}
}
