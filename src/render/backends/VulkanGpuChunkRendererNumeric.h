#pragma once

#include "CoreTypes.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <volk.h>

namespace ve::rendering
{
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
}
