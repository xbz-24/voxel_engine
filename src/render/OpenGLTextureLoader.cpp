#include "TextureLoader.h"

#include "CoreTypes.h"

namespace ve::rendering
{
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, image.rgba.data());
		return TextureHandle{ texture };
	}

	GLuint NativeOpenGLTexture(TextureHandle handle) noexcept
	{
		return ve::core::NumericCast<GLuint>(handle.value);
	}

	float PackedRgbUnitChannel(int hex_color, int bit_shift) noexcept
	{
		return ve::core::ToFloat((hex_color >> bit_shift) & 0xFF) / 255.0f;
	}

	TextureHandle LoadTexture(const char* path)
	{
		return UploadOpenGLTexture(DecodeImageFile(path));
	}

	void SetColorFromHex(int hex_color)
	{
		const float red = PackedRgbUnitChannel(hex_color, 16);
		const float green = PackedRgbUnitChannel(hex_color, 8);
		const float blue = PackedRgbUnitChannel(hex_color, 0);
		glColor3f(red, green, blue);
	}
}
