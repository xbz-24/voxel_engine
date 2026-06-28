#include "VulkanSoftwareVoxelRasterizer.h"

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
			return static_cast<std::uint8_t>(std::clamp(static_cast<float>(value) * scale, 0.0f, 255.0f));
		}

		[[nodiscard]] std::uint32_t DarkenPacked(std::uint32_t packed, VkFormat format, float strength) noexcept
		{
			const bool bgra = IsBgraFormat(format);
			const std::uint8_t c0 = static_cast<std::uint8_t>(packed & 0xffu);
			const std::uint8_t c1 = static_cast<std::uint8_t>((packed >> 8u) & 0xffu);
			const std::uint8_t c2 = static_cast<std::uint8_t>((packed >> 16u) & 0xffu);
			const Rgb color = bgra ? Rgb{ c2, c1, c0 } : Rgb{ c0, c1, c2 };
			return PackColor({ DarkenChannel(color.r, strength), DarkenChannel(color.g, strength), DarkenChannel(color.b, strength) }, format);
		}

		[[nodiscard]] int ColorDelta(std::uint32_t left, std::uint32_t right) noexcept
		{
			const int b0 = static_cast<int>(left & 0xffu) - static_cast<int>(right & 0xffu);
			const int b1 = static_cast<int>((left >> 8u) & 0xffu) - static_cast<int>((right >> 8u) & 0xffu);
			const int b2 = static_cast<int>((left >> 16u) & 0xffu) - static_cast<int>((right >> 16u) & 0xffu);
			return std::abs(b0) + std::abs(b1) + std::abs(b2);
		}
	}

	void VulkanSoftwareVoxelRasterizer::ApplyVoxelOutlines(VkFormat format, float strength)
	{
		if (strength <= 0.01f || render_pixels_.empty() || render_extent_.width < 3u || render_extent_.height < 3u) return;
		outline_pixels_.resize(render_pixels_.size());
		std::copy(render_pixels_.begin(), render_pixels_.end(), outline_pixels_.begin());

		const int threshold = 72;
		for (std::uint32_t y = 1; y + 1u < render_extent_.height; ++y)
		{
			for (std::uint32_t x = 1; x + 1u < render_extent_.width; ++x)
			{
				const std::size_t index = static_cast<std::size_t>(y) * render_extent_.width + x;
				const std::uint32_t center = render_pixels_[index];
				const int dx = ColorDelta(center, render_pixels_[index - 1u]) + ColorDelta(center, render_pixels_[index + 1u]);
				const int dy = ColorDelta(center, render_pixels_[index - render_extent_.width]) + ColorDelta(center, render_pixels_[index + render_extent_.width]);
				if (std::max(dx, dy) > threshold) outline_pixels_[index] = DarkenPacked(center, format, strength);
			}
		}
		render_pixels_.swap(outline_pixels_);
	}
}
