
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
