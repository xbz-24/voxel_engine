#include "TextureLoader.h"

#include "Logger.h"

#include <stb_image.h>
#include <cstring>
#include <string>

namespace ve::rendering
{
	bool DecodedImage::IsValid() const noexcept
	{
		const std::size_t expected_byte_count =
			static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4u;
		return width > 0 && height > 0 && mip_level_count > 0 &&
			pixel_format == ImagePixelFormat::Rgba8 &&
			rgba.size() == expected_byte_count;
	}

	DecodedImage DecodeImageFile(const char* path)
	{
		int width = 0;
		int height = 0;
		int sourceChannels = 0;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path, &width, &height, &sourceChannels, 4);
		if (!data)
		{
			ve::log::Error(std::string("Failed to load texture: ") + path);
			return {};
		}

		VE_LOG_INFO("Texture " + std::string(path) + ": " + std::to_string(width) + "x" + std::to_string(height));
		DecodedImage image;
		image.width = width;
		image.height = height;
		image.source_channel_count = sourceChannels;
		const std::size_t byte_count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4u;
		image.rgba.resize(byte_count);
		std::memcpy(image.rgba.data(), data, byte_count);
		stbi_image_free(data);
		return image;
	}

	TextureHandle UploadOpenGLTexture(const DecodedImage& image)
	{
		if (!image.IsValid()) return kInvalidTextureHandle;

		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.rgba.data());
		return TextureHandle{ texture };
	}

	GLuint NativeOpenGLTexture(TextureHandle handle) noexcept
	{
		return static_cast<GLuint>(handle.value);
	}

	TextureHandle LoadTexture(const char* path)
	{
		return UploadOpenGLTexture(DecodeImageFile(path));
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
