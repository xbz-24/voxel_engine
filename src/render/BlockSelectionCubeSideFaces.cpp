#include "BlockSelectionCube.h"

#include "TextureLoader.h"

/**
 * Draws the right face using the side texture.
 *
 * @param halfSize Half-size of the emitted cube face.
 */
void BlockSelectionCube::DrawRight(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, ve::rendering::NativeOpenGLTexture(_right));
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);
	glEnd();
}

/**
 * Draws the left face using the side texture.
 *
 * @param halfSize Half-size of the emitted cube face.
 */
void BlockSelectionCube::DrawLeft(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, ve::rendering::NativeOpenGLTexture(_left));
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);
	glEnd();
}
