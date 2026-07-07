#pragma once

#include "BlockSolidColorPalette.h"

#include <algorithm>
#include <cstdint>

namespace ve::blocks
{
	[[nodiscard]] inline float SolidColorHash01(int x, int y, int z) noexcept
	{
		std::uint32_t value = static_cast<std::uint32_t>(x) * 0x9E3779B9u;
		value ^= static_cast<std::uint32_t>(y) * 0x85EBCA6Bu;
		value ^= static_cast<std::uint32_t>(z) * 0xC2B2AE35u;
		value ^= value >> 16u;
		value *= 0x7FEB352Du;
		value ^= value >> 15u;
		return static_cast<float>(value & 0xFFFFu) / 65535.0f;
	}

	[[nodiscard]] inline SolidBlockColor JitteredSolidColor(BlockId block, int x, int y, int z) noexcept
	{
		SolidBlockColor color = SolidColor(block);
		float amplitude = 0.10f;
		if (block == BlockId::Grass || block == BlockId::OakLeaves || block == BlockId::BirchLeaves || block == BlockId::MossBlock) amplitude = 0.18f;
		if (block == BlockId::SeaLantern || block == BlockId::Glass || block == BlockId::Water) amplitude = 0.04f;
		const float jitter = (SolidColorHash01(x, y, z) - 0.5f) * amplitude;
		color.r = std::clamp(color.r + jitter, 0.0f, 1.0f);
		color.g = std::clamp(color.g + jitter, 0.0f, 1.0f);
		color.b = std::clamp(color.b + jitter, 0.0f, 1.0f);
		return color;
	}
}
