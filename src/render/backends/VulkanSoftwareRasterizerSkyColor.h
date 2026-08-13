#pragma once

#include "VulkanSoftwareRasterizerColorBlend.h"

namespace ve::rendering
{
	[[nodiscard]] inline Rgb SkyColor(float direction_y) noexcept
	{
		const float t = std::clamp((direction_y + 0.20f) * 0.80f, 0.0f, 1.0f);
		return Mix({ 78, 107, 150 }, { 166, 210, 255 }, t);
	}
}
