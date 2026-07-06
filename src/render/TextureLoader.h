#pragma once

#include <GL/glew.h>

#include "GraphicsTypes.h"
#include "RenderResources.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ve::rendering
{
	class RenderBackend;

	enum class ImagePixelFormat
	{
		Unknown,
		Rgba8
	};

	enum class TextureColorSpace
	{
		Linear,
		Srgb
	};

	/** CPU-side decoded RGBA8 image data. */
	struct DecodedImage
	{
		int width = 0;
		int height = 0;
		int source_channel_count = 0;
		int mip_level_count = 1;
		ImagePixelFormat pixel_format = ImagePixelFormat::Rgba8;
		TextureColorSpace color_space = TextureColorSpace::Srgb;
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

	/**
	 * Uploads decoded pixels through a backend-owned texture resource factory.
	 *
	 * @param backend Backend that owns the created texture resource.
	 * @param image CPU-side decoded image.
	 * @return Backend-owned texture resource, or null when unsupported/invalid.
	 */
	[[nodiscard]] RenderTexturePtr UploadTexture(RenderBackend& backend, const DecodedImage& image);

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
