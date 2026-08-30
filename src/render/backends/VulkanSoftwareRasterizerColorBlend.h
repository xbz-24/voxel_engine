#pragma once

#include "VulkanSoftwareRasterizerColorConversion.h"

#include <algorithm>

namespace ve::rendering
{
	[[nodiscard]] inline std::uint8_t Scale(std::uint8_t value, float amount) noexcept
	{
		const float scaled = std::clamp(ColorChannelFloat(value) * amount, 0.0f, 255.0f);
		return ColorChannelByte(scaled);
	}

	[[nodiscard]] inline Rgb Multiply(Rgb color, Rgb tint, float amount) noexcept
	{
		const float t = std::clamp(amount, 0.0f, 1.0f);
		const auto channel = [t](std::uint8_t value, std::uint8_t tint_value)
		{
			const float tinted = (ColorChannelFloat(value) * ColorChannelFloat(tint_value)) / 255.0f;
			return ColorChannelByte((ColorChannelFloat(value) * (1.0f - t)) + (tinted * t));
		};
		return { channel(color.r, tint.r), channel(color.g, tint.g), channel(color.b, tint.b) };
	}

	[[nodiscard]] inline Rgb Mix(Rgb left, Rgb right, float amount) noexcept
	{
		const float t = std::clamp(amount, 0.0f, 1.0f);
		const auto lerp = [t](std::uint8_t a, std::uint8_t b)
		{
			return ColorChannelByte((ColorChannelFloat(a) * (1.0f - t)) + (ColorChannelFloat(b) * t));
		};
		return { lerp(left.r, right.r), lerp(left.g, right.g), lerp(left.b, right.b) };
	}
}
