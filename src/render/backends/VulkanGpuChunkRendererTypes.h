#pragma once

#include "CoreTypes.h"

#include <volk.h>

#include <algorithm>
#include <cstddef>
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

	[[nodiscard]] inline glm::length_t GlmAxis(int axis) noexcept
	{
		return ve::core::NumericCast<glm::length_t>(axis);
	}

	[[nodiscard]] inline float VertexCoordinate(int coordinate) noexcept
	{
		return ve::core::ToFloat(coordinate);
	}

	[[nodiscard]] inline float ExtentFloat(std::uint32_t value) noexcept
	{
		return ve::core::ToFloat(value);
	}

	template <typename Value>
	[[nodiscard]] inline double TimestampValueDouble(Value value) noexcept
	{
		return ve::core::ToDouble(value);
	}

	[[nodiscard]] inline std::uint32_t RenderElementCount(std::size_t count) noexcept
	{
		return ve::core::ToU32(count);
	}

	[[nodiscard]] inline std::uint32_t VertexAttributeOffset(std::size_t byte_offset) noexcept
	{
		return ve::core::ToU32(byte_offset);
	}

	[[nodiscard]] inline std::uint32_t TimestampQueryIndex(std::size_t frame_index) noexcept
	{
		return ve::core::ToU32(frame_index * 2u);
	}

	[[nodiscard]] inline std::uint32_t TimestampQueryCount(std::size_t frame_count) noexcept
	{
		return ve::core::ToU32(frame_count * 2u);
	}

	[[nodiscard]] constexpr std::size_t ChunkColumnCount(int width, int depth) noexcept
	{
		return ve::core::ToIndex(width * depth);
	}

	[[nodiscard]] constexpr std::size_t WorldColumnCount(int width_blocks, int depth_blocks) noexcept
	{
		return ve::core::ToIndex(std::max(width_blocks, 0)) *
			ve::core::ToIndex(std::max(depth_blocks, 0));
	}

	[[nodiscard]] inline VkDeviceSize VulkanByteSize(std::size_t byte_count) noexcept
	{
		return ve::core::NumericCast<VkDeviceSize>(byte_count);
	}

	[[nodiscard]] inline std::size_t HostByteSize(VkDeviceSize byte_count) noexcept
	{
		return ve::core::ToIndex(byte_count);
	}

	[[nodiscard]] inline std::uint8_t PackUnorm8(float value) noexcept
	{
		return ve::core::ToU8((std::clamp(value, 0.0f, 1.0f) * 255.0f) + 0.5f);
	}

	[[nodiscard]] inline std::uint32_t PackedVertexByte(std::uint8_t value) noexcept
	{
		return ve::core::ToU32(value);
	}

	[[nodiscard]] inline std::uint32_t PackVoxelColor(float red, float green, float blue, float alpha) noexcept
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
}
