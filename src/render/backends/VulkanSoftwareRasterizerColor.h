#pragma once


#include "CoreTypes.h"

#include <algorithm>
#include <cstdint>
#include <vulkan/vulkan.h>

namespace ve::rendering
{
	struct Rgb
	{
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
	};

	[[nodiscard]] inline bool IsBgraFormat(VkFormat format) noexcept
	{
		return format == VK_FORMAT_B8G8R8A8_UNORM ||
			format == VK_FORMAT_B8G8R8A8_SRGB ||
			format == VK_FORMAT_B8G8R8A8_SNORM;
	}

	[[nodiscard]] inline std::uint32_t ColorBits(std::uint8_t value) noexcept
	{
		return ve::core::ToU32(value);
	}

	[[nodiscard]] inline std::uint8_t ColorByte(std::uint32_t value) noexcept
	{
		return ve::core::ToU8(value);
	}

	[[nodiscard]] inline float ColorChannelFloat(std::uint8_t value) noexcept
	{
		return ve::core::ToFloat(value);
	}

	[[nodiscard]] inline std::uint8_t ColorChannelByte(float value) noexcept
	{
		return ve::core::ToU8(value);
	}

	[[nodiscard]] inline int PackedChannelInt(std::uint32_t packed, std::uint32_t shift) noexcept
	{
		return ve::core::ToInt((packed >> shift) & 0xffu);
	}

	[[nodiscard]] inline std::uint32_t PackColor(Rgb color, VkFormat format) noexcept
	{
		constexpr std::uint32_t alpha = 255u;
		if (IsBgraFormat(format))
		{
			return (alpha << 24u) |
				(ColorBits(color.r) << 16u) |
				(ColorBits(color.g) << 8u) |
				ColorBits(color.b);
		}
		return (alpha << 24u) |
			(ColorBits(color.b) << 16u) |
			(ColorBits(color.g) << 8u) |
			ColorBits(color.r);
	}

	[[nodiscard]] inline std::uint8_t Scale(std::uint8_t value, float amount) noexcept
	{
		const float scaled = std::clamp(ColorChannelFloat(value) * amount, 0.0f, 255.0f);
		return ColorChannelByte(scaled);
	}

	[[nodiscard]] inline std::uint32_t PackRgb(Rgb color) noexcept
	{
		return (ColorBits(color.r) << 16u) |
			(ColorBits(color.g) << 8u) |
			ColorBits(color.b);
	}

	[[nodiscard]] inline Rgb UnpackRgb(std::uint32_t packed) noexcept
	{
		return {
			ColorByte((packed >> 16u) & 0xffu),
			ColorByte((packed >> 8u) & 0xffu),
			ColorByte(packed & 0xffu)
		};
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

	[[nodiscard]] inline Rgb SkyColor(float direction_y) noexcept
	{
		const float t = std::clamp((direction_y + 0.20f) * 0.80f, 0.0f, 1.0f);
		return Mix({ 78, 107, 150 }, { 166, 210, 255 }, t);
	}
}
