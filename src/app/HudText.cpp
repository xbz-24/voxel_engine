#include "HudRenderer.h"

namespace ve::ui
{
	void HudRenderer::DrawText(const std::string& text, float x, float y, float scale)
	{
		glBindTexture(GL_TEXTURE_2D, textures_.font);
		glBegin(GL_QUADS);
		const float charSize = 10.0f * scale;
		const float uvStep = 1.0f / 16.0f;
		float cursorX = x;
		for (char c : text)
		{
			if (c == ' ')
			{
				cursorX += charSize * 0.8f;
				continue;
			}

			const int asciiCode = static_cast<int>(c);
			const int col = asciiCode % 16;
			const int row = 15 - (asciiCode / 16);
			const float uvX = static_cast<float>(col) * uvStep;
			const float uvY = static_cast<float>(row) * uvStep;
			glTexCoord2f(uvX, uvY + uvStep); glVertex2f(cursorX, y);
			glTexCoord2f(uvX + uvStep, uvY + uvStep); glVertex2f(cursorX + charSize, y);
			glTexCoord2f(uvX + uvStep, uvY); glVertex2f(cursorX + charSize, y + charSize);
			glTexCoord2f(uvX, uvY); glVertex2f(cursorX, y + charSize);
			cursorX += charSize * 0.8f;
		}
		glEnd();
	}
}
