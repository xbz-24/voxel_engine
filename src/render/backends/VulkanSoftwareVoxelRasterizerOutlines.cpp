#include "VulkanSoftwareVoxelRasterizer.h"

#include "CoreTypes.h"
#include "VulkanSoftwareRasterizerColor.h"

#include <algorithm>
#include <cstdlib>

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] std::uint8_t DarkenChannel(std::uint8_t value, float strength) noexcept
		{
			const float scale = 1.0f - std::clamp(strength, 0.0f, 0.85f);
			return ColorChannelByte(std::clamp(ColorChannelFloat(value) * scale, 0.0f, 255.0f));
		}

		[[nodiscard]] std::uint32_t DarkenPacked(std::uint32_t packed, VkFormat format, float strength) noexcept
		{
			const bool bgra = IsBgraFormat(format);
			const std::uint8_t first_channel = ColorByte(packed & 0xffu);
			const std::uint8_t second_channel = ColorByte((packed >> 8u) & 0xffu);
			const std::uint8_t third_channel = ColorByte((packed >> 16u) & 0xffu);
			const Rgb color = bgra ?
				Rgb{ third_channel, second_channel, first_channel } :
				Rgb{ first_channel, second_channel, third_channel };
			return PackColor({ DarkenChannel(color.r, strength), DarkenChannel(color.g, strength), DarkenChannel(color.b, strength) }, format);
		}

		[[nodiscard]] int ColorDelta(std::uint32_t left, std::uint32_t right) noexcept
		{
			const int first_channel_delta = PackedChannelInt(left, 0u) - PackedChannelInt(right, 0u);
			const int second_channel_delta = PackedChannelInt(left, 8u) - PackedChannelInt(right, 8u);
			const int third_channel_delta = PackedChannelInt(left, 16u) - PackedChannelInt(right, 16u);
			return std::abs(first_channel_delta) +
				std::abs(second_channel_delta) +
				std::abs(third_channel_delta);
		}
	}

	void VulkanSoftwareVoxelRasterizer::ApplyVoxelOutlines(VkFormat format, float strength)
	{
		if (strength <= 0.01f || render_pixels_.empty() || render_extent_.width < 3u || render_extent_.height < 3u) return;
		outline_pixels_.resize(render_pixels_.size());
		std::copy(render_pixels_.begin(), render_pixels_.end(), outline_pixels_.begin());

		const int threshold = 72;
		for (std::uint32_t pixel_y = 1; pixel_y + 1u < render_extent_.height; ++pixel_y)
		{
			for (std::uint32_t pixel_x = 1; pixel_x + 1u < render_extent_.width; ++pixel_x)
			{
				const std::size_t pixel_index =
					(ve::core::ToIndex(pixel_y) * ve::core::ToIndex(render_extent_.width)) +
					ve::core::ToIndex(pixel_x);
				const std::uint32_t center = render_pixels_[pixel_index];
				const int horizontal_delta =
					ColorDelta(center, render_pixels_[pixel_index - 1u]) +
					ColorDelta(center, render_pixels_[pixel_index + 1u]);
				const int vertical_delta =
					ColorDelta(center, render_pixels_[pixel_index - render_extent_.width]) +
					ColorDelta(center, render_pixels_[pixel_index + render_extent_.width]);
				if (std::max(horizontal_delta, vertical_delta) > threshold)
				{
					outline_pixels_[pixel_index] = DarkenPacked(center, format, strength);
				}
			}
		}
		render_pixels_.swap(outline_pixels_);
	}
}
