#include "BlockSelectionCube.h"

/**
 * Draws the front face using the loaded side texture.
 *
 * @param halfSize Half-size of the emitted cube.
 */
void BlockSelectionCube::DrawFront(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, _front);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);
	glEnd();
}

/**
 * Draws the back face using the loaded side texture.
 *
 * @param halfSize Half-size of the emitted cube.
 */
void BlockSelectionCube::DrawBack(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, _back);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);
	glEnd();
}

/**
 * Draws the top face using the loaded grass texture.
 *
 * @param halfSize Half-size of the emitted cube.
 */
void BlockSelectionCube::DrawTop(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, _top);
	glColor3f(0.404f, 0.655f, 0.239f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, halfSize, -halfSize);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
}

/**
 * Draws the bottom face using the loaded dirt texture.
 *
 * @param halfSize Half-size of the emitted cube.
 */
void BlockSelectionCube::DrawBottom(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, _bottom);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, -halfSize, halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, -halfSize, halfSize);
	glEnd();
}
