#include "VulkanSoftwareVoxelRasterizer.h"

#include "VulkanSoftwareRasterizerColor.h"

#include <algorithm>
#include <array>
#include <cstdio>

namespace ve::rendering
{
	namespace
	{
		std::array<std::uint8_t, 7> GlyphFor(char value) noexcept
		{
			switch (value)
			{
			case '0': return { { 0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110 } };
			case '1': return { { 0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110 } };
			case '2': return { { 0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111 } };
			case '3': return { { 0b11110, 0b00001, 0b00001, 0b01110, 0b00001, 0b00001, 0b11110 } };
			case '4': return { { 0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010 } };
			case '5': return { { 0b11111, 0b10000, 0b10000, 0b11110, 0b00001, 0b00001, 0b11110 } };
			case '6': return { { 0b01110, 0b10000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110 } };
			case '7': return { { 0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000 } };
			case '8': return { { 0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110 } };
			case '9': return { { 0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00001, 0b01110 } };
			case 'A': return { { 0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 } };
			case 'C': return { { 0b01111, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b01111 } };
			case 'D': return { { 0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110 } };
			case 'E': return { { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111 } };
			case 'F': return { { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000 } };
			case 'G': return { { 0b01111, 0b10000, 0b10000, 0b10111, 0b10001, 0b10001, 0b01111 } };
			case 'H': return { { 0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 } };
			case 'I': return { { 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111 } };
			case 'K': return { { 0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001 } };
			case 'L': return { { 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111 } };
			case 'M': return { { 0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001 } };
			case 'N': return { { 0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001 } };
			case 'O': return { { 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 } };
			case 'P': return { { 0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000 } };
			case 'R': return { { 0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001 } };
			case 'S': return { { 0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110 } };
			case 'T': return { { 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100 } };
			case 'U': return { { 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 } };
			case 'V': return { { 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b01010, 0b00100 } };
			case 'X': return { { 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b01010, 0b10001 } };
			case 'Y': return { { 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100 } };
			case '.': return { { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b01100, 0b01100 } };
			case '-': return { { 0b00000, 0b00000, 0b00000, 0b11111, 0b00000, 0b00000, 0b00000 } };
			default: return { { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000 } };
			}
		}
	}
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
	void VulkanSoftwareVoxelRasterizer::DrawText(const char* text, std::uint32_t x, std::uint32_t y, std::uint32_t scale, std::uint32_t color)
	{
		std::uint32_t cursor_x = x;
		for (const char* cursor = text; *cursor != '\0'; ++cursor)
		{
			const std::array<std::uint8_t, 7> glyph = GlyphFor(*cursor);
			for (std::size_t row_index = 0; row_index < glyph.size(); ++row_index)
			{
				const std::uint8_t row_bits = glyph[row_index];
				for (std::uint32_t column = 0; column < 5u; ++column)
				{
					const std::uint8_t mask = static_cast<std::uint8_t>(1u << (4u - column));
					if ((row_bits & mask) == 0u) continue;

					const std::uint32_t pixel_x = cursor_x + (column * scale);
					const std::uint32_t pixel_y = y + (static_cast<std::uint32_t>(row_index) * scale);
					for (std::uint32_t dy = 0; dy < scale; ++dy)
					{
						if (pixel_y + dy >= render_extent_.height) continue;
						std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(pixel_y + dy) * render_extent_.width);
						for (std::uint32_t dx = 0; dx < scale; ++dx)
						{
							if (pixel_x + dx < render_extent_.width) row[pixel_x + dx] = color;
						}
					}
				}
			}
			cursor_x += 6u * scale;
			if (cursor_x >= render_extent_.width) return;
		}
	}
	void VulkanSoftwareVoxelRasterizer::DrawFilledRect(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height, std::uint32_t color)
	{
		if (x >= render_extent_.width || y >= render_extent_.height || width == 0u || height == 0u) return;
		const std::uint32_t end_x = std::min(x + width, render_extent_.width);
		const std::uint32_t end_y = std::min(y + height, render_extent_.height);
		for (std::uint32_t row_y = y; row_y < end_y; ++row_y)
		{
			std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(row_y) * render_extent_.width);
			std::fill(row + x, row + end_x, color);
		}
	}
	bool VulkanSoftwareVoxelRasterizer::Slider(const VulkanSoftwareVoxelRasterizerFrame& frame,
		const char* label,
		float& value,
		float min_value,
		float max_value,
		std::uint32_t x,
		std::uint32_t y,
		std::uint32_t width)
	{
		const std::uint32_t text = PackColor({ 232, 248, 244 }, frame.format);
		const std::uint32_t rail = PackColor({ 43, 64, 75 }, frame.format);
		const std::uint32_t fill = PackColor({ 87, 220, 180 }, frame.format);
		const std::uint32_t handle = PackColor({ 246, 245, 210 }, frame.format);
		DrawText(label, x, y, 1u, text);
		const std::uint32_t rail_x = x + 104u;
		const std::uint32_t rail_y = y + 2u;
		const std::uint32_t rail_width = std::min(width, render_extent_.width > rail_x ? render_extent_.width - rail_x : 0u);
		if (rail_width == 0u) return false;
		DrawFilledRect(rail_x, rail_y + 4u, rail_width, 5u, rail);

		const float range = std::max(max_value - min_value, 0.001f);
		const double mouse_x = extent_.width == 0u ? 0.0 : frame.input.mouse_x * (static_cast<double>(render_extent_.width) / static_cast<double>(extent_.width));
		const double mouse_y = extent_.height == 0u ? 0.0 : frame.input.mouse_y * (static_cast<double>(render_extent_.height) / static_cast<double>(extent_.height));
		const bool hovered = mouse_x >= rail_x && mouse_x <= static_cast<double>(rail_x + rail_width) &&
			mouse_y >= rail_y && mouse_y <= static_cast<double>(rail_y + 14u);
		bool changed = false;
		if (hovered && frame.input.mouse_left_down)
		{
			const float t = static_cast<float>((mouse_x - static_cast<double>(rail_x)) / static_cast<double>(rail_width));
			value = min_value + (std::clamp(t, 0.0f, 1.0f) * range);
			changed = true;
		}

		const float normalized = std::clamp((value - min_value) / range, 0.0f, 1.0f);
		const std::uint32_t filled = static_cast<std::uint32_t>(normalized * static_cast<float>(rail_width));
		DrawFilledRect(rail_x, rail_y + 4u, filled, 5u, fill);
		const std::uint32_t knob_x = rail_x + std::min(filled, rail_width > 1u ? rail_width - 1u : 0u);
		DrawFilledRect(knob_x > 2u ? knob_x - 2u : knob_x, rail_y, 5u, 13u, handle);

		std::array<char, 24> value_text{};
		std::snprintf(value_text.data(), value_text.size(), "%.1f", value);
		DrawText(value_text.data(), rail_x + rail_width + 12u, y, 1u, text);
		return changed;
	}
	void VulkanSoftwareVoxelRasterizer::DrawTuningPanel(const VulkanSoftwareVoxelRasterizerFrame& frame)
	{
		const std::uint32_t panel_width = std::min<std::uint32_t>(520u, render_extent_.width > 24u ? render_extent_.width - 24u : render_extent_.width);
		const std::uint32_t panel_height = 150u;
		const std::uint32_t panel_x = render_extent_.width > panel_width + 20u ? render_extent_.width - panel_width - 20u : 0u;
		const std::uint32_t panel_y = render_extent_.height > panel_height + 20u ? 20u : 0u;
		const std::uint32_t bg = PackColor({ 16, 24, 32 }, frame.format);
		const std::uint32_t border = PackColor({ 87, 220, 180 }, frame.format);
		const std::uint32_t text = PackColor({ 232, 248, 244 }, frame.format);
		DrawFilledRect(panel_x, panel_y, panel_width, panel_height, bg);
		DrawFilledRect(panel_x, panel_y, panel_width, 3u, border);
		DrawFilledRect(panel_x, panel_y + panel_height - 3u, panel_width, 3u, border);
		DrawFilledRect(panel_x, panel_y, 3u, panel_height, border);
		DrawFilledRect(panel_x + panel_width - 3u, panel_y, 3u, panel_height, border);
		DrawText("VULKAN DEMO TUNING", panel_x + 12u, panel_y + 10u, 1u, text);

		float res_x = static_cast<float>(frame.settings.max_internal_width);
		float res_y = static_cast<float>(frame.settings.max_internal_height);
		float pixel = static_cast<float>(frame.settings.pixel_block_size);
		float ray = frame.settings.max_ray_distance;
		float fog = frame.settings.fog_strength;
		float outline = frame.settings.outline_strength;
		const std::uint32_t slider_x = panel_x + 12u;
		const std::uint32_t slider_w = std::min<std::uint32_t>(220u, panel_width > 250u ? panel_width - 250u : 120u);
		Slider(frame, "RES X", res_x, 320.0f, 1920.0f, slider_x, panel_y + 31u, slider_w);
		Slider(frame, "RES Y", res_y, 180.0f, 1080.0f, slider_x, panel_y + 49u, slider_w);
		Slider(frame, "PIXEL", pixel, 1.0f, 8.0f, slider_x, panel_y + 67u, slider_w);
		Slider(frame, "RAY", ray, 32.0f, 144.0f, slider_x, panel_y + 85u, slider_w);
		Slider(frame, "FOG", fog, 0.0f, 0.9f, slider_x, panel_y + 103u, slider_w);
		Slider(frame, "OUTLINE", outline, 0.0f, 0.65f, slider_x, panel_y + 121u, slider_w);
		frame.settings.max_internal_width = static_cast<std::uint32_t>(std::clamp(res_x, 320.0f, 1920.0f));
		frame.settings.max_internal_height = static_cast<std::uint32_t>(std::clamp(res_y, 180.0f, 1080.0f));
		frame.settings.pixel_block_size = static_cast<std::uint32_t>(std::round(std::clamp(pixel, 1.0f, 8.0f)));
		frame.settings.max_ray_distance = std::clamp(ray, 32.0f, 144.0f);
		frame.settings.fog_strength = std::clamp(fog, 0.0f, 0.9f);
		frame.settings.outline_strength = std::clamp(outline, 0.0f, 0.65f);
	}
	void VulkanSoftwareVoxelRasterizer::DrawDemoOverlay(const VulkanSoftwareVoxelRasterizerFrame& frame)
	{
		const std::uint32_t accent = PackColor({ 87, 220, 180 }, frame.format);
		const std::uint32_t shadow = PackColor({ 18, 32, 44 }, frame.format);
		const std::uint32_t text = PackColor({ 232, 248, 244 }, frame.format);
		const std::uint32_t desired_bar_width = render_extent_.width > 480u ? std::min<std::uint32_t>(render_extent_.width / 3u, 430u) : render_extent_.width;
		const std::uint32_t bar_y = render_extent_.height > 32u ? 14u : 0u;
		const std::uint32_t bar_x = std::min<std::uint32_t>(20u, render_extent_.width);
		const std::uint32_t bar_width = std::min(desired_bar_width, render_extent_.width - bar_x);
		const std::uint32_t bar_end_x = std::min(bar_x + bar_width, render_extent_.width);
		for (std::uint32_t y = bar_y; y < bar_y + 96u && y < render_extent_.height; ++y)
		{
			std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(y) * render_extent_.width);
			std::fill(row + bar_x, row + bar_end_x, shadow);
		}
		for (std::uint32_t y = bar_y; y < bar_y + 4u && y < render_extent_.height; ++y)
		{
			std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(y) * render_extent_.width);
			std::fill(row + bar_x, row + bar_end_x, accent);
		}

		std::array<char, 64> fps_line{};
		std::array<char, 64> cpu_line{};
		std::array<char, 64> vk_line{};
		std::array<char, 64> resolution_line{};
		std::array<char, 64> sampling_line{};
		const double milliseconds = std::max(frame.delta_seconds, 0.0) * 1000.0;
		if (frame.displayed_fps > 0)
		{
			std::snprintf(fps_line.data(), fps_line.size(), "FPS %d   %.1f MS", frame.displayed_fps, milliseconds);
		}
		else
		{
			std::snprintf(fps_line.data(), fps_line.size(), "FPS --   %.1f MS", milliseconds);
		}
		std::snprintf(cpu_line.data(), cpu_line.size(), "SNAP %.1f RAST %.1f UI %.1f", frame.previous_timing.snapshot_cpu_ms, frame.previous_timing.raster_cpu_ms, frame.previous_timing.upscale_cpu_ms);
		std::snprintf(vk_line.data(), vk_line.size(), "COPY %.1f VK %.1f", frame.previous_timing.upload_cpu_ms, frame.previous_timing.present_cpu_ms);
		std::snprintf(resolution_line.data(), resolution_line.size(), "OUT %uX%u IN %uX%u",
			extent_.width,
			extent_.height,
			frame.previous_timing.render_extent.width,
			frame.previous_timing.render_extent.height);
		std::snprintf(sampling_line.data(), sampling_line.size(), "STEP %u TH %u",
			frame.previous_timing.sample_step,
			frame.previous_timing.worker_count);

		DrawText("VULKAN VOXEL DEMO", bar_x + 8u, bar_y + 9u, 1u, text);
		DrawText(fps_line.data(), bar_x + 8u, bar_y + 23u, 1u, text);
		DrawText(cpu_line.data(), bar_x + 8u, bar_y + 37u, 1u, text);
		DrawText(vk_line.data(), bar_x + 8u, bar_y + 51u, 1u, text);
		if (frame.previous_timing.has_gpu_copy_timing)
		{
			std::array<char, 64> gpu_line{};
			std::snprintf(gpu_line.data(), gpu_line.size(), "GPU COPY %.2f", frame.previous_timing.gpu_copy_ms);
			DrawText(gpu_line.data(), bar_x + 8u, bar_y + 65u, 1u, text);
		}
		else
		{
			DrawText("GPU COPY --", bar_x + 8u, bar_y + 65u, 1u, text);
		}
		DrawText(resolution_line.data(), bar_x + 8u, bar_y + 79u, 1u, text);
		DrawText(sampling_line.data(), bar_x + 8u, bar_y + 93u, 1u, text);
	}
}
