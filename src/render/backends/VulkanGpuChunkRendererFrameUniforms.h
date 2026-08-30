#pragma once

#include "VulkanGpuChunkRendererFrameSanitization.h"
#include "VulkanGpuChunkRendererShadowMath.h"
#include "VoxelRenderStyle.h"

#include <volk.h>

namespace ve::rendering
{
	[[nodiscard]] inline VulkanVoxelFrameUniforms PackVulkanVoxelFrameUniforms(
		const VoxelRenderStyle& style,
		glm::vec3 camera_position,
		glm::vec3 camera_forward,
		float elapsed_seconds,
		VkExtent2D viewport_extent) noexcept
	{
		using detail::FiniteInRange;
		using detail::FiniteNonNegative;
		camera_position = detail::FinitePosition(camera_position);
		const glm::vec3 sun_direction = NormalizeVulkanShaderDirection(
			style.sun_direction, { -0.42f, 0.78f, -0.46f });
		const float fog_start = FiniteNonNegative(style.fog_start_distance, 165.0f);
		const float fog_end = std::max(
			FiniteNonNegative(style.fog_end_distance, 455.0f), fog_start + 0.001f);
		const glm::vec3 sun_color = detail::FiniteNonNegativeColor(
			style.sun_color, { 1.16f, 1.04f, 0.84f });
		const glm::vec3 sky_horizon = detail::FiniteNonNegativeColor(
			style.sky_horizon_color, { 0.72f, 0.70f, 0.64f });
		const glm::vec3 sky_zenith = detail::FiniteNonNegativeColor(
			style.sky_zenith_color, { 0.52f, 0.68f, 0.88f });
		const float width = std::max(ve::core::ToFloat(viewport_extent.width), 1.0f);
		const float height = std::max(ve::core::ToFloat(viewport_extent.height), 1.0f);
		const float shadow_distance = std::isfinite(style.directional_shadow_distance)
			? std::clamp(style.directional_shadow_distance, 32.0f, 512.0f)
			: 220.0f;

		return VulkanVoxelFrameUniforms{
			glm::vec4{ camera_position, FiniteNonNegative(elapsed_seconds, 0.0f) },
			glm::vec4{ sun_direction, FiniteNonNegative(style.sun_intensity, 1.0f) },
			glm::vec4{ sun_color, std::max(FiniteNonNegative(style.exposure, 1.0f), 0.001f) },
			glm::vec4{ sky_horizon, fog_start },
			glm::vec4{ sky_zenith, fog_end },
			glm::vec4{ width, height, 1.0f / width, 1.0f / height },
			glm::vec4{
				FiniteInRange(style.fog_strength, 0.42f, 1.0f),
				FiniteInRange(style.cloud_coverage, 0.42f, 1.0f),
				FiniteInRange(style.cloud_density, 0.62f, 1.0f),
				FiniteNonNegative(style.cloud_speed, 0.018f) },
			glm::vec4{
				FiniteInRange(style.surface_detail_strength, 1.0f, 2.0f),
				FiniteInRange(style.water_reflection_strength, 1.0f, 2.0f),
				FiniteInRange(style.shadow_strength, 1.0f, 2.0f),
				FiniteInRange(style.specular_strength, 1.0f, 2.0f) },
			glm::vec4{
				FiniteInRange(style.ambient_occlusion_strength, 1.0f, 2.0f),
				FiniteInRange(style.aerial_perspective_strength, 1.0f, 2.0f),
				FiniteInRange(style.subsurface_scattering_strength, 1.0f, 2.0f),
				FiniteInRange(style.cloud_light_absorption, 1.0f, 2.0f) },
			glm::vec4{
				FiniteInRange(style.directional_shadow_strength, 0.85f, 2.0f),
				FiniteInRange(style.directional_shadow_softness, 1.0f, 2.0f),
				shadow_distance,
				1.0f / ve::core::ToFloat(VulkanDirectionalShadowMapResolution) },
			BuildVulkanDirectionalShadowMatrix(
				camera_position, camera_forward, sun_direction, shadow_distance)
		};
	}
}
