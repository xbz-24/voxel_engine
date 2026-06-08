#include "Utilities.h"
#include "Logger.h"

#include <string>
#include <stb_image.h> 

GLuint Utils::load_texture(const char* path) 
{

    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 4);

    if (!data) 
    {
        ve::log::Error(std::string("Failed to load texture: ") + path);
        return 0;
    }

    const int memoryUsage = width * height * 4;
    ve::log::Info(
        std::string("Texture ") + path +
        ": " + std::to_string(width) + "x" + std::to_string(height) +
        ", " + std::to_string(nrChannels) + " channels (forced to 4), ~" +
        std::to_string(memoryUsage) + " bytes");

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return texture;
}
void Utils::setHexColor(int hex) 
{
    float r = ((hex >> 16) & 0xFF) / 255.0f;
    float g = ((hex >> 8) & 0xFF) / 255.0f;
    float b = (hex & 0xFF) / 255.0f;
    glColor3f(r, g, b);
}
