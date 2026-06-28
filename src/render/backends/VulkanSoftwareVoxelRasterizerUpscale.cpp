#include "VulkanSoftwareVoxelRasterizer.h"

#include "VulkanSoftwareRasterizerColor.h"

#include <algorithm>
#include <cstring>

namespace ve::rendering
{
	namespace
	{
		std::uint32_t CeilDiv(std::uint64_t numerator, std::uint32_t denominator) noexcept
		{
			return static_cast<std::uint32_t>((numerator + static_cast<std::uint64_t>(denominator) - 1ull) / denominator);
		}

		std::uint8_t DarkenChannel(std::uint8_t value, float strength) noexcept
		{
			const float scale = 1.0f - std::clamp(strength, 0.0f, 0.85f);
			return static_cast<std::uint8_t>(std::clamp(static_cast<float>(value) * scale, 0.0f, 255.0f));
		}

		std::uint32_t DarkenPacked(std::uint32_t packed, VkFormat format, float strength) noexcept
		{
			const bool bgra = IsBgraFormat(format);
			const std::uint8_t c0 = static_cast<std::uint8_t>(packed & 0xffu);
			const std::uint8_t c1 = static_cast<std::uint8_t>((packed >> 8u) & 0xffu);
			const std::uint8_t c2 = static_cast<std::uint8_t>((packed >> 16u) & 0xffu);
			const Rgb color = bgra ? Rgb{ c2, c1, c0 } : Rgb{ c0, c1, c2 };
			return PackColor({ DarkenChannel(color.r, strength), DarkenChannel(color.g, strength), DarkenChannel(color.b, strength) }, format);
		}

		int ColorDelta(std::uint32_t left, std::uint32_t right) noexcept
		{
			const int b0 = static_cast<int>(left & 0xffu) - static_cast<int>(right & 0xffu);
			const int b1 = static_cast<int>((left >> 8u) & 0xffu) - static_cast<int>((right >> 8u) & 0xffu);
			const int b2 = static_cast<int>((left >> 16u) & 0xffu) - static_cast<int>((right >> 16u) & 0xffu);
			return std::abs(b0) + std::abs(b1) + std::abs(b2);
		}

	}
	void VulkanSoftwareVoxelRasterizer::RebuildUpscaleLookup()
	{
		upscale_x_ranges_.resize(render_extent_.width);
		for (std::uint32_t x = 0; x < render_extent_.width; ++x)
		{
			upscale_x_ranges_[x] = {
				CeilDiv(static_cast<std::uint64_t>(x) * extent_.width, render_extent_.width),
				CeilDiv(static_cast<std::uint64_t>(x + 1u) * extent_.width, render_extent_.width)
			};
		}

		upscale_y_ranges_.resize(render_extent_.height);
		for (std::uint32_t y = 0; y < render_extent_.height; ++y)
		{
			upscale_y_ranges_[y] = {
				CeilDiv(static_cast<std::uint64_t>(y) * extent_.height, render_extent_.height),
				CeilDiv(static_cast<std::uint64_t>(y + 1u) * extent_.height, render_extent_.height)
			};
		}
	}
	void VulkanSoftwareVoxelRasterizer::UpscaleRenderPixels()
	{
		if (render_extent_.width == extent_.width && render_extent_.height == extent_.height)
		{
			std::copy(render_pixels_.begin(), render_pixels_.end(), pixels_.begin());
			return;
		}
		if (upscale_x_ranges_.size() != render_extent_.width || upscale_y_ranges_.size() != render_extent_.height)
		{
			RebuildUpscaleLookup();
		}
		const std::size_t row_bytes = static_cast<std::size_t>(extent_.width) * sizeof(std::uint32_t);
		for (std::uint32_t source_y = 0; source_y < render_extent_.height; ++source_y)
		{
			const UpscaleRange y_range = upscale_y_ranges_[source_y];
			if (y_range.begin >= y_range.end) continue;

			const std::uint32_t* source_row = render_pixels_.data() + (static_cast<std::size_t>(source_y) * render_extent_.width);
			std::uint32_t* first_destination_row = pixels_.data() + (static_cast<std::size_t>(y_range.begin) * extent_.width);
			for (std::uint32_t source_x = 0; source_x < render_extent_.width; ++source_x)
			{
				const UpscaleRange x_range = upscale_x_ranges_[source_x];
				if (x_range.begin < x_range.end)
				{
					std::fill(first_destination_row + x_range.begin, first_destination_row + x_range.end, source_row[source_x]);
				}
			}

			for (std::uint32_t destination_y = y_range.begin + 1u; destination_y < y_range.end; ++destination_y)
			{
				std::uint32_t* destination_row = pixels_.data() + (static_cast<std::size_t>(destination_y) * extent_.width);
				std::memcpy(destination_row, first_destination_row, row_bytes);
			}
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
				if (std::max(dx, dy) > threshold)
				{
					outline_pixels_[index] = DarkenPacked(center, format, strength);
				}
			}
		}
		render_pixels_.swap(outline_pixels_);
	}
}
