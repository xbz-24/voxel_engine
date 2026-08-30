#pragma once

#include "CoreTypes.h"

#include <algorithm>
#include <cstdint>
#include <glm/glm.hpp>

namespace ve::rendering
{
	[[nodiscard]] inline std::uint8_t PackUnorm8(float value) noexcept
	{
		return ve::core::ToU8((std::clamp(value, 0.0f, 1.0f) * 255.0f) + 0.5f);
	}

	[[nodiscard]] inline std::uint32_t PackedVertexByte(std::uint8_t value) noexcept
	{
		return ve::core::ToU32(value);
	}

	[[nodiscard]] inline std::uint32_t PackVoxelColor(
		float red, float green, float blue, float alpha) noexcept
	{
		return PackedVertexByte(PackUnorm8(red)) |
			(PackedVertexByte(PackUnorm8(green)) << 8U) |
			(PackedVertexByte(PackUnorm8(blue)) << 16U) |
			(PackedVertexByte(PackUnorm8(alpha)) << 24U);
	}

	[[nodiscard]] inline std::uint8_t PackSnorm8(int value) noexcept
	{
		const int clamped_value = std::clamp(value, -1, 1);
		return ve::core::ToU8(clamped_value < 0 ? -127 : clamped_value * 127);
	}

	[[nodiscard]] inline std::uint32_t PackVoxelNormal(glm::ivec3 normal) noexcept
	{
		return PackedVertexByte(PackSnorm8(normal.x)) |
			(PackedVertexByte(PackSnorm8(normal.y)) << 8U) |
			(PackedVertexByte(PackSnorm8(normal.z)) << 16U) |
			(0x7fU << 24U);
	}

	[[nodiscard]] inline glm::vec3 UnpackVoxelNormal(std::uint32_t packed_normal) noexcept
	{
		const auto unpack_component = [packed_normal](std::uint32_t shift) noexcept
		{
			const std::uint32_t component = (packed_normal >> shift) & 0xffU;
			const int signed_component = component <= 127U
				? ve::core::ToInt(component)
				: ve::core::ToInt(component) - 256;
			return ve::core::ToFloat(signed_component) / 127.0f;
		};
		return { unpack_component(0U), unpack_component(8U), unpack_component(16U) };
	}
}
