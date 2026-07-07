#pragma once

#include <algorithm>
#include <cstdint>
#include <glm/glm.hpp>

namespace ve::rendering
{
	struct VoxelVertex
	{
		glm::vec3 position{ 0.0f };
		std::uint32_t color_rgba8 = 0xffffffffU;
		float light = 1.0f;
		std::uint32_t normal_snorm8 = 0x7f000000U;
	};

	[[nodiscard]] inline std::uint8_t PackUnorm8(float value) noexcept
	{
		return static_cast<std::uint8_t>((std::clamp(value, 0.0f, 1.0f) * 255.0f) + 0.5f);
	}

	[[nodiscard]] inline std::uint32_t PackVoxelColor(float red, float green, float blue, float alpha) noexcept
	{
		return static_cast<std::uint32_t>(PackUnorm8(red)) |
			(static_cast<std::uint32_t>(PackUnorm8(green)) << 8U) |
			(static_cast<std::uint32_t>(PackUnorm8(blue)) << 16U) |
			(static_cast<std::uint32_t>(PackUnorm8(alpha)) << 24U);
	}

	[[nodiscard]] inline std::uint8_t PackSnorm8(int value) noexcept
	{
		const int clamped_value = std::clamp(value, -1, 1);
		return static_cast<std::uint8_t>(clamped_value < 0 ? -127 : clamped_value * 127);
	}

	[[nodiscard]] inline std::uint32_t PackVoxelNormal(glm::ivec3 normal) noexcept
	{
		return static_cast<std::uint32_t>(PackSnorm8(normal.x)) |
			(static_cast<std::uint32_t>(PackSnorm8(normal.y)) << 8U) |
			(static_cast<std::uint32_t>(PackSnorm8(normal.z)) << 16U) |
			(0x7fU << 24U);
	}
}
