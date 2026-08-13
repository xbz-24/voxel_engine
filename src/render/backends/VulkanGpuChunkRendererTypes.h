#pragma once


#include "CoreTypes.h"
#include "VoxelRenderStyle.h"

#include <volk.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <type_traits>

namespace ve::rendering
{
	inline constexpr std::uint32_t VulkanDirectionalShadowMapResolution = 2048U;

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
		glm::vec4 advanced_lighting_parameters{};
		glm::vec4 directional_shadow_parameters{};
		glm::mat4 light_view_projection{ 1.0f };
	};

	static_assert(std::is_standard_layout_v<VulkanVoxelTransformPushConstants>);
	static_assert(std::is_standard_layout_v<VulkanVoxelFrameUniforms>);
	static_assert(sizeof(VulkanVoxelTransformPushConstants) == 64U);
	static_assert(offsetof(VulkanVoxelFrameUniforms, sun_direction_and_intensity) == 16U);
	static_assert(offsetof(VulkanVoxelFrameUniforms, atmosphere_parameters) == 96U);
	static_assert(offsetof(VulkanVoxelFrameUniforms, surface_parameters) == 112U);
	static_assert(offsetof(VulkanVoxelFrameUniforms, advanced_lighting_parameters) == 128U);
	static_assert(offsetof(VulkanVoxelFrameUniforms, directional_shadow_parameters) == 144U);
	static_assert(offsetof(VulkanVoxelFrameUniforms, light_view_projection) == 160U);
	static_assert(sizeof(VulkanVoxelFrameUniforms) == 224U);

	[[nodiscard]] inline glm::vec3 NormalizeVulkanShaderDirection(
		glm::vec3 direction,
		glm::vec3 fallback) noexcept
	{
		const bool direction_is_finite = std::isfinite(direction.x) &&
			std::isfinite(direction.y) && std::isfinite(direction.z);
		float largest_component = direction_is_finite
			? std::max(std::abs(direction.x), std::max(std::abs(direction.y), std::abs(direction.z)))
			: 0.0f;
		if (largest_component <= 0.000001f)
		{
			direction = fallback;
			largest_component = std::max(
				std::abs(direction.x),
				std::max(std::abs(direction.y), std::abs(direction.z)));
		}
		const glm::vec3 scaled_direction = direction / largest_component;
		return scaled_direction / std::sqrt(glm::dot(scaled_direction, scaled_direction));
	}

	[[nodiscard]] inline glm::mat4 BuildVulkanDirectionalShadowMatrix(
		glm::vec3 camera_position,
		glm::vec3 camera_forward,
		glm::vec3 sun_direction,
		float shadow_distance) noexcept
	{
		camera_forward = NormalizeVulkanShaderDirection(camera_forward, { 0.0f, 0.0f, -1.0f });
		sun_direction = NormalizeVulkanShaderDirection(sun_direction, { -0.42f, 0.78f, -0.46f });
		const glm::vec3 shadow_center = camera_position + camera_forward * (shadow_distance * 0.32f);
		const glm::vec3 light_position = shadow_center + sun_direction * (shadow_distance * 1.75f);
		const glm::vec3 world_up = std::abs(glm::dot(sun_direction, glm::vec3{ 0.0f, 1.0f, 0.0f })) > 0.96f
			? glm::vec3{ 0.0f, 0.0f, 1.0f }
			: glm::vec3{ 0.0f, 1.0f, 0.0f };
		const glm::mat4 light_view = glm::lookAtRH(light_position, shadow_center, world_up);
		glm::mat4 light_projection = glm::orthoRH_ZO(
			-shadow_distance,
			shadow_distance,
			-shadow_distance,
			shadow_distance,
			0.1f,
			shadow_distance * 4.0f);
		light_projection[1][1] *= -1.0f;
		const glm::mat4 unsnapped_light_matrix = light_projection * light_view;
		const glm::vec4 world_origin_clip = unsnapped_light_matrix * glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
		const float half_resolution = ve::core::ToFloat(VulkanDirectionalShadowMapResolution) * 0.5f;
		const glm::vec2 origin_texels = glm::vec2{ world_origin_clip } * half_resolution;
		const glm::vec2 rounded_origin_texels = glm::round(origin_texels);
		const glm::vec2 clip_offset = (rounded_origin_texels - origin_texels) / half_resolution;
		light_projection[3][0] += clip_offset.x;
		light_projection[3][1] += clip_offset.y;
		return light_projection * light_view;
	}

	[[nodiscard]] inline VulkanVoxelFrameUniforms PackVulkanVoxelFrameUniforms(
		const VoxelRenderStyle& style,
		glm::vec3 camera_position,
		glm::vec3 camera_forward,
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

		const glm::vec3 sun_direction = NormalizeVulkanShaderDirection(
			style.sun_direction,
			{ -0.42f, 0.78f, -0.46f });

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
		const float directional_shadow_distance = std::isfinite(style.directional_shadow_distance)
			? std::clamp(style.directional_shadow_distance, 32.0f, 512.0f)
			: 220.0f;
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
				finite_in_range(style.specular_strength, 1.0f, 2.0f) },
			glm::vec4{
				finite_in_range(style.ambient_occlusion_strength, 1.0f, 2.0f),
				finite_in_range(style.aerial_perspective_strength, 1.0f, 2.0f),
				finite_in_range(style.subsurface_scattering_strength, 1.0f, 2.0f),
				finite_in_range(style.cloud_light_absorption, 1.0f, 2.0f) },
			glm::vec4{
				finite_in_range(style.directional_shadow_strength, 0.85f, 2.0f),
				finite_in_range(style.directional_shadow_softness, 1.0f, 2.0f),
				directional_shadow_distance,
				1.0f / ve::core::ToFloat(VulkanDirectionalShadowMapResolution) },
			BuildVulkanDirectionalShadowMatrix(
				camera_position,
				camera_forward,
				sun_direction,
				directional_shadow_distance)
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

	[[nodiscard]] inline glm::vec3 UnpackVoxelNormal(std::uint32_t packed_normal) noexcept
	{
		const auto unpack_component = [packed_normal](std::uint32_t shift) noexcept
		{
			const std::uint32_t unsigned_component = (packed_normal >> shift) & 0xffU;
			const int signed_component = unsigned_component <= 127U
				? ve::core::ToInt(unsigned_component)
				: ve::core::ToInt(unsigned_component) - 256;
			return ve::core::ToFloat(signed_component) / 127.0f;
		};
		return {
			unpack_component(0U),
			unpack_component(8U),
			unpack_component(16U)
		};
	}
}
