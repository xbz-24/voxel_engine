#include "Utilities.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint Utils::load_texture(const char* path) {

        int width, height, nrChannels;

        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

        if (!data) {
            printf("Failed to load texture: %s\n", path);
            return 0;
        }

        int memoryUsage = width * height * nrChannels;
        printf("Texture %s: %dx%d, %d channels, ~%d bytes\n",
            path, width, height, nrChannels, memoryUsage);

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLint format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);

        return texture;
}