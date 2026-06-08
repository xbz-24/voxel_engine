#include "BlockSelectionCube.h"

/**
 * Draws the right face using the loaded side texture.
 *
 * @param halfSize Half-size of the emitted cube.
 */
void BlockSelectionCube::DrawRight(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, _right);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);
	glEnd();
}

/**
 * Draws the left face using the loaded side texture.
 *
 * @param halfSize Half-size of the emitted cube.
 */
void BlockSelectionCube::DrawLeft(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, _left);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);
	glEnd();
}
