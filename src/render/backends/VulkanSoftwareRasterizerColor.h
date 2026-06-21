#pragma once

#include "BlockDefinitions.h"

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

	[[nodiscard]] inline Rgb BaseBlockColor(ve::blocks::BlockId block) noexcept
	{
		using enum ve::blocks::BlockId;
		if (block == Grass) return { 92, 159, 74 };
		if (block == Dirt) return { 112, 76, 45 };
		if (block == Stone) return { 124, 124, 124 };
		if (block == Cobblestone) return { 105, 105, 105 };
		if (block == OakLog || block == BirchLog || block == SpruceLog) return { 123, 89, 50 };
		if (block == OakPlanks || block == BirchPlanks || block == SprucePlanks) return { 174, 134, 82 };
		if (block == Sand) return { 214, 198, 129 };
		if (block == Bricks) return { 151, 74, 60 };
		if (block == DiamondOre) return { 92, 196, 211 };
		if (block == CoalOre) return { 54, 54, 54 };
		if (block == IronOre) return { 216, 170, 130 };
		if (block == Obsidian) return { 42, 32, 65 };
		if (block == OakLeaves || block == BirchLeaves) return { 67, 137, 58 };
		if (block == Snow) return { 235, 242, 246 };
		if (block == MossBlock) return { 73, 126, 58 };
		if (block == Pumpkin) return { 198, 112, 33 };
		if (block == Melon) return { 104, 158, 53 };
		if (block == HayBlock) return { 203, 174, 63 };
		if (block == AmethystBlock) return { 137, 89, 178 };
		if (block == Glass) return { 148, 214, 219 };
		return { 132, 132, 132 };
	}

	[[nodiscard]] inline Rgb ApplyBlockTint(ve::blocks::BlockId block, ve::blocks::BlockFace face, Rgb color) noexcept
	{
		if (block == ve::blocks::BlockId::Grass && face == ve::blocks::BlockFace::Top)
		{
			return Multiply(color, { 96, 178, 68 }, 0.68f);
		}
		if (block == ve::blocks::BlockId::OakLeaves || block == ve::blocks::BlockId::BirchLeaves)
		{
			return Multiply(color, { 72, 160, 68 }, 0.58f);
		}
		return color;
	}
}
