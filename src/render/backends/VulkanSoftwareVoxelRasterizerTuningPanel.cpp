#include "VulkanSoftwareVoxelRasterizer.h"

#include "VulkanSoftwareRasterizerColor.h"

#include <algorithm>
#include <cmath>

namespace ve::rendering
{
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
}
