#pragma once

#include <GL/glew.h>

namespace ve::rendering
{
	/**
	 * Loads an image file into an OpenGL 2D texture.
	 *
	 * @param path Null-terminated file-system path to the image file.
	 * @return OpenGL texture id. Returns 0 when the image cannot be decoded or uploaded.
	 */
	GLuint LoadTexture(const char* path);

	/**
	 * Sets the current immediate-mode OpenGL color from a packed RGB value.
	 *
	 * @param hexColor RGB color encoded as 0xRRGGBB.
	 */
	void SetColorFromHex(int hexColor);
}
