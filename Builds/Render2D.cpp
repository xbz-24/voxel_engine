#include "Render2D.h"

namespace ve::rendering
{
	void DrawTexturedQuad(GLuint texture, float x, float y, float width, float height)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width, y);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y + height);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y + height);
		glEnd();
	}
}

