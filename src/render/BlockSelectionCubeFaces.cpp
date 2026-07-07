#include "BlockSelectionCube.h"

#include "TextureLoader.h"

/**
 * Draws the front face using the side texture.
 *
 * @param halfSize Half-size of the emitted cube face.
 */
void BlockSelectionCube::DrawFront(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, ve::rendering::NativeOpenGLTexture(_front));
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);
	glEnd();
}

/**
 * Draws the back face using the side texture.
 *
 * @param halfSize Half-size of the emitted cube face.
 */
void BlockSelectionCube::DrawBack(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, ve::rendering::NativeOpenGLTexture(_back));
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);
	glEnd();
}

/**
 * Draws the top face using the grass texture.
 *
 * @param halfSize Half-size of the emitted cube face.
 */
void BlockSelectionCube::DrawTop(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, ve::rendering::NativeOpenGLTexture(_top));
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
 * Draws the bottom face using the dirt texture.
 *
 * @param halfSize Half-size of the emitted cube face.
 */
void BlockSelectionCube::DrawBottom(float halfSize) const
{
	glBindTexture(GL_TEXTURE_2D, ve::rendering::NativeOpenGLTexture(_bottom));
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, -halfSize, halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, -halfSize, halfSize);
	glEnd();
}
