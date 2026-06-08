#pragma once

#include <GL/glew.h>

namespace ve::rendering
{
	/**
	 * Draws a screen-space textured quad using the active projection/modelview.
	 *
	 * @param texture OpenGL texture id.
	 * @param x Left screen coordinate.
	 * @param y Top screen coordinate.
	 * @param width Quad width in pixels.
	 * @param height Quad height in pixels.
	 */
	void DrawTexturedQuad(GLuint texture, float x, float y, float width, float height);
}

