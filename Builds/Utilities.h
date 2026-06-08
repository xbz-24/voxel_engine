#pragma once

#include <GL/glew.h>

namespace Utils
{
    /**
     * Loads an image file into an OpenGL 2D texture.
     *
     * @param path Texture file path.
     * @return OpenGL texture id, or 0 when loading fails.
     */
    GLuint load_texture(const char* path);

    /**
     * Sets the current OpenGL color from a 0xRRGGBB integer.
     *
     * @param hex RGB color encoded as 0xRRGGBB.
     */
    void setHexColor(int hex);
}
