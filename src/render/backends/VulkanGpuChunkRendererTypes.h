#pragma once

#include "CoreTypes.h"
#include "VoxelRenderStyle.h"

#include <volk.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <glm/glm.hpp>
#include <type_traits>

namespace ve::rendering
{
	struct VoxelVertex
	{
		glm::vec3 position{ 0.0f };
		std::uint32_t color_rgba8 = 0xffffffffU;
		float light = 1.0f;
		std::uint32_t normal_snorm8 = 0x7f000000U;
	};

	/** Portable matrix-only push-constant contract shared by voxel and sky pipelines. */
	struct alignas(16) VulkanVoxelTransformPushConstants
	{
		glm::mat4 transform{ 1.0f };
	};

	/** std140-compatible dynamic frame data consumed by fragment shaders. */
	struct alignas(16) VulkanVoxelFrameUniforms
	{
		glm::vec4 camera_position_and_time{};
		glm::vec4 sun_direction_and_intensity{};
		glm::vec4 sun_color_and_exposure{};
		glm::vec4 sky_horizon_color_and_fog_start{};
		glm::vec4 sky_zenith_color_and_fog_end{};
		glm::vec4 viewport_size_and_inverse{};
		glm::vec4 atmosphere_parameters{};
		glm::vec4 surface_parameters{};
	};

	static_assert(std::is_standard_layout_v<VulkanVoxelTransformPushConstants>);
	static_assert(std::is_standard_layout_v<VulkanVoxelFrameUniforms>);
	static_assert(sizeof(VulkanVoxelTransformPushConstants) == 64U);
	static_assert(offsetof(VulkanVoxelFrameUniforms, sun_direction_and_intensity) == 16U);
	static_assert(offsetof(VulkanVoxelFrameUniforms, atmosphere_parameters) == 96U);
	static_assert(offsetof(VulkanVoxelFrameUniforms, surface_parameters) == 112U);
	static_assert(sizeof(VulkanVoxelFrameUniforms) == 128U);

	[[nodiscard]] inline VulkanVoxelFrameUniforms PackVulkanVoxelFrameUniforms(
		const VoxelRenderStyle& style,
		glm::vec3 camera_position,
		float elapsed_seconds,
		VkExtent2D viewport_extent) noexcept
	{
		const auto finite_non_negative = [](float value, float fallback) noexcept
		{
			return std::isfinite(value) && value >= 0.0f ? value : fallback;
		};
		const auto finite_non_negative_color = [&finite_non_negative](glm::vec3 color, glm::vec3 fallback) noexcept
		{
			return glm::vec3{
				finite_non_negative(color.x, fallback.x),
				finite_non_negative(color.y, fallback.y),
				finite_non_negative(color.z, fallback.z)
			};
		};

		glm::vec3 sun_direction = style.sun_direction;
		const bool direction_is_finite = std::isfinite(sun_direction.x) &&
			std::isfinite(sun_direction.y) && std::isfinite(sun_direction.z);
		float largest_direction_component = direction_is_finite
			? std::max(std::abs(sun_direction.x), std::max(std::abs(sun_direction.y), std::abs(sun_direction.z)))
			: 0.0f;
		if (largest_direction_component <= 0.000001f)
		{
			sun_direction = { -0.42f, 0.78f, -0.46f };
			largest_direction_component = 0.78f;
		}
		const glm::vec3 scaled_direction = sun_direction / largest_direction_component;
		sun_direction = scaled_direction / std::sqrt(glm::dot(scaled_direction, scaled_direction));

		const auto finite_in_range = [](float value, float fallback, float maximum) noexcept
		{
			return std::isfinite(value) ? std::clamp(value, 0.0f, maximum) : fallback;
		};
		camera_position.x = std::isfinite(camera_position.x) ? camera_position.x : 0.0f;
		camera_position.y = std::isfinite(camera_position.y) ? camera_position.y : 0.0f;
		camera_position.z = std::isfinite(camera_position.z) ? camera_position.z : 0.0f;
		const float frame_time = finite_non_negative(elapsed_seconds, 0.0f);
		const float sun_intensity = finite_non_negative(style.sun_intensity, 1.0f);
		const float exposure = std::max(finite_non_negative(style.exposure, 1.0f), 0.001f);
		const float fog_start_distance = finite_non_negative(style.fog_start_distance, 165.0f);
		const float fog_end_distance = std::max(
			finite_non_negative(style.fog_end_distance, 455.0f),
			fog_start_distance + 0.001f);
		const glm::vec3 sun_color = finite_non_negative_color(
			style.sun_color,
			{ 1.16f, 1.04f, 0.84f });
		const glm::vec3 sky_horizon_color = finite_non_negative_color(
			style.sky_horizon_color,
			{ 0.72f, 0.70f, 0.64f });
		const glm::vec3 sky_zenith_color = finite_non_negative_color(
			style.sky_zenith_color,
			{ 0.52f, 0.68f, 0.88f });
		const float viewport_width = std::max(ve::core::ToFloat(viewport_extent.width), 1.0f);
		const float viewport_height = std::max(ve::core::ToFloat(viewport_extent.height), 1.0f);
		return VulkanVoxelFrameUniforms{
			glm::vec4{ camera_position, frame_time },
			glm::vec4{ sun_direction, sun_intensity },
			glm::vec4{ sun_color, exposure },
			glm::vec4{ sky_horizon_color, fog_start_distance },
			glm::vec4{ sky_zenith_color, fog_end_distance },
			glm::vec4{ viewport_width, viewport_height, 1.0f / viewport_width, 1.0f / viewport_height },
			glm::vec4{
				finite_in_range(style.fog_strength, 0.42f, 1.0f),
				finite_in_range(style.cloud_coverage, 0.42f, 1.0f),
				finite_in_range(style.cloud_density, 0.62f, 1.0f),
				finite_non_negative(style.cloud_speed, 0.018f) },
			glm::vec4{
				finite_in_range(style.surface_detail_strength, 1.0f, 2.0f),
				finite_in_range(style.water_reflection_strength, 1.0f, 2.0f),
				finite_in_range(style.shadow_strength, 1.0f, 2.0f),
				finite_in_range(style.specular_strength, 1.0f, 2.0f) }
		};
	}

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
