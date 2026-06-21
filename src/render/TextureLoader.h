#pragma once

#include <GL/glew.h>

#include "GraphicsTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ve::rendering
{
	// TODO: Introduce TextureResource/TextureUploader interfaces so decoded pixels are shared and upload remains backend-specific.
	/** CPU-side decoded RGBA8 image data. */
	struct DecodedImage
	{
		int width = 0;
		int height = 0;
		std::vector<std::uint8_t> rgba;

		[[nodiscard]] bool IsValid() const noexcept;
	};

	/**
	 * Decodes an image file into CPU memory without creating GPU resources.
	 *
	 * @param path Null-terminated file-system path to the image file.
	 * @return Decoded image. Empty data means decoding failed.
	 */
	[[nodiscard]] DecodedImage DecodeImageFile(const char* path);

	/**
	 * Uploads decoded RGBA8 pixels into an OpenGL 2D texture.
	 *
	 * @param image CPU-side decoded image.
	 * @return Backend-neutral texture handle wrapping the OpenGL texture registry id.
	 */
	[[nodiscard]] TextureHandle UploadOpenGLTexture(const DecodedImage& image);

	/** @param handle Backend-neutral handle. @return Native OpenGL texture id for compatibility renderer binding. */
	[[nodiscard]] GLuint NativeOpenGLTexture(TextureHandle handle) noexcept;

	/**
	 * Loads an image file into an OpenGL 2D texture.
	 *
	 * @param path Null-terminated file-system path to the image file.
	 * @return Backend-neutral texture handle. Returns invalid when the image cannot be decoded or uploaded.
	 */
	TextureHandle LoadTexture(const char* path);

	/**
	 * Sets the current immediate-mode OpenGL color from a packed RGB value.
	 *
	 * @param hexColor RGB color encoded as 0xRRGGBB.
	 */
	void SetColorFromHex(int hexColor);
}
