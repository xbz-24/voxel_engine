#include "TextureLoader.h"

#include "CoreTypes.h"
#include "Logger.h"
#include "RenderBackend.h"
#include "StbiImageData.h"

#include <cstring>
#include <string>

namespace ve::rendering
{
	bool DecodedImage::IsValid() const noexcept
	{
		const std::size_t expected_byte_count =
			ve::core::ToIndex(width) * ve::core::ToIndex(height) * 4u;
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
		StbiImageData data(stbi_load(path, &width, &height, &sourceChannels, 4));
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
		const std::size_t byte_count = ve::core::ToIndex(width) * ve::core::ToIndex(height) * 4u;
		image.rgba.resize(byte_count);
		std::memcpy(image.rgba.data(), data.get(), byte_count);
		return image;
	}

	RenderTexturePtr UploadTexture(RenderBackend& backend, const DecodedImage& image)
	{
		if (!image.IsValid())
		{
			return nullptr;
		}

		const RenderTextureDescriptor descriptor{
			ve::core::ToU32(image.width),
			ve::core::ToU32(image.height),
			ve::core::ToU32(image.mip_level_count),
			RenderTextureFormat::Rgba8,
			RenderTextureUsage::Sampled | RenderTextureUsage::TransferDestination
		};
		return backend.CreateTextureResource(RenderTextureUpload{ descriptor, image.rgba });
	}

}
