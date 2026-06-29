#include "VulkanSoftwareVoxelRasterizer.h"

#include "VulkanSoftwareRasterizerColor.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>

namespace ve::rendering
{
	bool VulkanSoftwareVoxelRasterizer::Slider(const VulkanSoftwareVoxelRasterizerFrame& frame,
		const char* label,
		float& value,
		float min_value,
		float max_value,
		std::uint32_t origin_x,
		std::uint32_t origin_y,
		std::uint32_t control_width)
	{
		const std::uint32_t text = PackColor({ 232, 248, 244 }, frame.format);
		const std::uint32_t rail = PackColor({ 43, 64, 75 }, frame.format);
		const std::uint32_t fill = PackColor({ 87, 220, 180 }, frame.format);
		const std::uint32_t handle = PackColor({ 246, 245, 210 }, frame.format);
		DrawText(label, origin_x, origin_y, 1u, text);
		const std::uint32_t rail_x = origin_x + 104u;
		const std::uint32_t rail_y = origin_y + 2u;
		const std::uint32_t rail_width = std::min(
			control_width,
			render_extent_.width > rail_x ? render_extent_.width - rail_x : 0u);
		if (rail_width == 0u) return false;

		DrawFilledRect(rail_x, rail_y + 4u, rail_width, 5u, rail);
		const float range = std::max(max_value - min_value, 0.001f);
		const double mouse_scale_x = extent_.width == 0u ?
			0.0 :
			static_cast<double>(render_extent_.width) / static_cast<double>(extent_.width);
		const double mouse_scale_y = extent_.height == 0u ?
			0.0 :
			static_cast<double>(render_extent_.height) / static_cast<double>(extent_.height);
		const double scaled_mouse_x = frame.input.mouse_x * mouse_scale_x;
		const double scaled_mouse_y = frame.input.mouse_y * mouse_scale_y;
		const bool hovered =
			scaled_mouse_x >= rail_x &&
			scaled_mouse_x <= static_cast<double>(rail_x + rail_width) &&
			scaled_mouse_y >= rail_y &&
			scaled_mouse_y <= static_cast<double>(rail_y + 14u);
		bool changed = false;
		if (hovered && frame.input.mouse_left_down)
		{
			const float drag_fraction =
				static_cast<float>((scaled_mouse_x - static_cast<double>(rail_x)) / static_cast<double>(rail_width));
			value = min_value + (std::clamp(drag_fraction, 0.0f, 1.0f) * range);
			changed = true;
		}

		const float normalized = std::clamp((value - min_value) / range, 0.0f, 1.0f);
		const std::uint32_t filled = static_cast<std::uint32_t>(normalized * static_cast<float>(rail_width));
		DrawFilledRect(rail_x, rail_y + 4u, filled, 5u, fill);
		const std::uint32_t knob_x = rail_x + std::min(filled, rail_width > 1u ? rail_width - 1u : 0u);
		DrawFilledRect(knob_x > 2u ? knob_x - 2u : knob_x, rail_y, 5u, 13u, handle);

		std::array<char, 24> value_text{};
		std::snprintf(value_text.data(), value_text.size(), "%.1f", value);
		DrawText(value_text.data(), rail_x + rail_width + 12u, origin_y, 1u, text);
		return changed;
	}
}
