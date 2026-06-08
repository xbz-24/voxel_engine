#include "TextureLoader.h"

#include "Logger.h"

#include <stb_image.h>
#include <string>

namespace ve::rendering
{
	/**
	 * Loads an image file into an OpenGL 2D texture.
	 *
	 * @param path Null-terminated file-system path to the image file.
	 * @return OpenGL texture id, or 0 when loading fails.
	 */
	GLuint LoadTexture(const char* path)
	{
		int width = 0;
		int height = 0;
		int sourceChannels = 0;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path, &width, &height, &sourceChannels, 4);
		if (!data)
		{
			ve::log::Error(std::string("Failed to load texture: ") + path);
			return 0;
		}

		VE_LOG_INFO("Texture " + std::string(path) + ": " + std::to_string(width) + "x" + std::to_string(height));
		GLuint texture = 0;
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

	/**
	 * Sets the current immediate-mode OpenGL color from a packed RGB value.
	 *
	 * @param hexColor RGB color encoded as 0xRRGGBB.
	 */
	void SetColorFromHex(int hexColor)
	{
		const float red = static_cast<float>((hexColor >> 16) & 0xFF) / 255.0f;
		const float green = static_cast<float>((hexColor >> 8) & 0xFF) / 255.0f;
		const float blue = static_cast<float>(hexColor & 0xFF) / 255.0f;
		glColor3f(red, green, blue);
	}
}
