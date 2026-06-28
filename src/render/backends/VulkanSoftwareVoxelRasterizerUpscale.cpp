#include "VulkanSoftwareVoxelRasterizer.h"

#include <algorithm>
#include <cstring>

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] std::uint32_t CeilDiv(std::uint64_t numerator, std::uint32_t denominator) noexcept
		{
			return static_cast<std::uint32_t>((numerator + static_cast<std::uint64_t>(denominator) - 1ull) / denominator);
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
				if (x_range.begin < x_range.end) std::fill(first_destination_row + x_range.begin, first_destination_row + x_range.end, source_row[source_x]);
			}

			for (std::uint32_t destination_y = y_range.begin + 1u; destination_y < y_range.end; ++destination_y)
			{
				std::uint32_t* destination_row = pixels_.data() + (static_cast<std::size_t>(destination_y) * extent_.width);
				std::memcpy(destination_row, first_destination_row, row_bytes);
			}
		}
	}
}
