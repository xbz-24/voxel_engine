#include "VulkanSoftwareVoxelRasterizer.h"

#include "VulkanSoftwareRasterizerColor.h"
#include "VulkanSoftwareVoxelRasterizerGlyphs.h"

#include <algorithm>

namespace ve::rendering
{
	void VulkanSoftwareVoxelRasterizer::DrawCrosshair(VkFormat format)
	{
		const std::uint32_t center_x = render_extent_.width / 2u;
		const std::uint32_t center_y = render_extent_.height / 2u;
		const std::uint32_t crosshair = PackColor({ 235, 235, 235 }, format);
		for (std::uint32_t offset = 0; offset < 8u; ++offset)
		{
			if (center_x + offset < render_extent_.width) render_pixels_[(center_y * render_extent_.width) + center_x + offset] = crosshair;
			if (center_x >= offset) render_pixels_[(center_y * render_extent_.width) + center_x - offset] = crosshair;
			if (center_y + offset < render_extent_.height) render_pixels_[((center_y + offset) * render_extent_.width) + center_x] = crosshair;
			if (center_y >= offset) render_pixels_[((center_y - offset) * render_extent_.width) + center_x] = crosshair;
		}
	}

	void VulkanSoftwareVoxelRasterizer::DrawText(
		const char* text,
		std::uint32_t origin_x,
		std::uint32_t origin_y,
		std::uint32_t scale,
		std::uint32_t color)
	{
		std::uint32_t cursor_x = origin_x;
		for (const char* cursor = text; *cursor != '\0'; ++cursor)
		{
			const std::array<std::uint8_t, 7> glyph = GlyphFor(*cursor);
			for (std::size_t row_index = 0; row_index < glyph.size(); ++row_index)
			{
				for (std::uint32_t column = 0; column < 5u; ++column)
				{
					const std::uint8_t mask = static_cast<std::uint8_t>(1u << (4u - column));
					if ((glyph[row_index] & mask) == 0u) continue;

					const std::uint32_t pixel_x = cursor_x + (column * scale);
					const std::uint32_t pixel_y = origin_y + (static_cast<std::uint32_t>(row_index) * scale);
					for (std::uint32_t pixel_offset_y = 0; pixel_offset_y < scale; ++pixel_offset_y)
					{
						if (pixel_y + pixel_offset_y >= render_extent_.height) continue;
						std::uint32_t* row = render_pixels_.data() +
							(static_cast<std::size_t>(pixel_y + pixel_offset_y) * render_extent_.width);
						for (std::uint32_t pixel_offset_x = 0; pixel_offset_x < scale; ++pixel_offset_x)
						{
							if (pixel_x + pixel_offset_x < render_extent_.width)
							{
								row[pixel_x + pixel_offset_x] = color;
							}
						}
					}
				}
			}
			cursor_x += 6u * scale;
			if (cursor_x >= render_extent_.width) return;
		}
	}

	void VulkanSoftwareVoxelRasterizer::DrawFilledRect(
		std::uint32_t origin_x,
		std::uint32_t origin_y,
		std::uint32_t width,
		std::uint32_t height,
		std::uint32_t color)
	{
		if (origin_x >= render_extent_.width ||
			origin_y >= render_extent_.height ||
			width == 0u ||
			height == 0u)
		{
			return;
		}
		const std::uint32_t end_x = std::min(origin_x + width, render_extent_.width);
		const std::uint32_t end_y = std::min(origin_y + height, render_extent_.height);
		for (std::uint32_t row_y = origin_y; row_y < end_y; ++row_y)
		{
			std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(row_y) * render_extent_.width);
			std::fill(row + origin_x, row + end_x, color);
		}
	}
}
