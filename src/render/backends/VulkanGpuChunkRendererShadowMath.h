#pragma once

#include "CoreTypes.h"
#include "VulkanGpuChunkRendererVertexTypes.h"

#include <algorithm>
#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace ve::rendering
{
	[[nodiscard]] inline glm::vec3 NormalizeVulkanShaderDirection(
		glm::vec3 direction,
		glm::vec3 fallback) noexcept
	{
		const bool is_finite = std::isfinite(direction.x) &&
			std::isfinite(direction.y) && std::isfinite(direction.z);
		float largest_component = is_finite
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
			-shadow_distance, shadow_distance,
			-shadow_distance, shadow_distance,
			0.1f, shadow_distance * 4.0f);
		light_projection[1][1] *= -1.0f;
		const glm::mat4 unsnapped_light_matrix = light_projection * light_view;
		const glm::vec4 origin_clip = unsnapped_light_matrix * glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
		const float half_resolution = ve::core::ToFloat(VulkanDirectionalShadowMapResolution) * 0.5f;
		const glm::vec2 origin_texels = glm::vec2{ origin_clip } * half_resolution;
		const glm::vec2 clip_offset = (glm::round(origin_texels) - origin_texels) / half_resolution;
		light_projection[3][0] += clip_offset.x;
		light_projection[3][1] += clip_offset.y;
		return light_projection * light_view;
	}
}
