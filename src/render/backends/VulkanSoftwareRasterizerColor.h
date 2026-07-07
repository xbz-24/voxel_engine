#pragma once

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

	[[nodiscard]] inline std::uint32_t PackColor(Rgb color, VkFormat format) noexcept
	{
		constexpr std::uint32_t alpha = 255u;
		if (IsBgraFormat(format))
		{
			return (alpha << 24u) |
				(static_cast<std::uint32_t>(color.r) << 16u) |
				(static_cast<std::uint32_t>(color.g) << 8u) |
				static_cast<std::uint32_t>(color.b);
		}
		return (alpha << 24u) |
			(static_cast<std::uint32_t>(color.b) << 16u) |
			(static_cast<std::uint32_t>(color.g) << 8u) |
			static_cast<std::uint32_t>(color.r);
	}

	[[nodiscard]] inline std::uint8_t Scale(std::uint8_t value, float amount) noexcept
	{
		const float scaled = std::clamp(static_cast<float>(value) * amount, 0.0f, 255.0f);
		return static_cast<std::uint8_t>(scaled);
	}

	[[nodiscard]] inline std::uint32_t PackRgb(Rgb color) noexcept
	{
		return (static_cast<std::uint32_t>(color.r) << 16u) |
			(static_cast<std::uint32_t>(color.g) << 8u) |
			static_cast<std::uint32_t>(color.b);
	}

	[[nodiscard]] inline Rgb UnpackRgb(std::uint32_t packed) noexcept
	{
		return {
			static_cast<std::uint8_t>((packed >> 16u) & 0xffu),
			static_cast<std::uint8_t>((packed >> 8u) & 0xffu),
			static_cast<std::uint8_t>(packed & 0xffu)
		};
	}

	[[nodiscard]] inline Rgb Multiply(Rgb color, Rgb tint, float amount) noexcept
	{
		const float t = std::clamp(amount, 0.0f, 1.0f);
		const auto channel = [t](std::uint8_t value, std::uint8_t tint_value)
		{
			const float tinted = (static_cast<float>(value) * static_cast<float>(tint_value)) / 255.0f;
			return static_cast<std::uint8_t>((static_cast<float>(value) * (1.0f - t)) + (tinted * t));
		};
		return { channel(color.r, tint.r), channel(color.g, tint.g), channel(color.b, tint.b) };
	}

	[[nodiscard]] inline Rgb Mix(Rgb left, Rgb right, float amount) noexcept
	{
		const float t = std::clamp(amount, 0.0f, 1.0f);
		const auto lerp = [t](std::uint8_t a, std::uint8_t b)
		{
			return static_cast<std::uint8_t>((static_cast<float>(a) * (1.0f - t)) + (static_cast<float>(b) * t));
		};
		return { lerp(left.r, right.r), lerp(left.g, right.g), lerp(left.b, right.b) };
	}

	[[nodiscard]] inline Rgb SkyColor(float direction_y) noexcept
	{
		const float t = std::clamp((direction_y + 0.20f) * 0.80f, 0.0f, 1.0f);
		return Mix({ 78, 107, 150 }, { 166, 210, 255 }, t);
	}
}
