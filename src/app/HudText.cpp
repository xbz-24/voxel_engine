#include "HudRenderer.h"

#include "CoreTypes.h"
#include "TextureLoader.h"

namespace ve::ui
{
	void HudRenderer::DrawText(const std::string& text, float x, float y, float scale)
	{
		glBindTexture(GL_TEXTURE_2D, ve::rendering::NativeOpenGLTexture(textures_.font));
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

			const int asciiCode = ve::core::ToInt(c);
			const int col = asciiCode % 16;
			const int row = 15 - (asciiCode / 16);
			const float uvX = ve::core::ToFloat(col) * uvStep;
			const float uvY = ve::core::ToFloat(row) * uvStep;
			glTexCoord2f(uvX, uvY + uvStep); glVertex2f(cursorX, y);
			glTexCoord2f(uvX + uvStep, uvY + uvStep); glVertex2f(cursorX + charSize, y);
			glTexCoord2f(uvX + uvStep, uvY); glVertex2f(cursorX + charSize, y + charSize);
			glTexCoord2f(uvX, uvY); glVertex2f(cursorX, y + charSize);
			cursorX += charSize * 0.8f;
		}
		glEnd();
	}
}
