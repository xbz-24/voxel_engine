#include "EngineConfigurationValidationDetail.h"
#include "VoxelRenderStyle.h"

#include <algorithm>
#include <cmath>

namespace ve::engine::configuration_validation
{
	namespace
	{
		[[nodiscard]] bool IsFinite(glm::vec3 value) noexcept
		{
			return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
		}

		[[nodiscard]] bool IsFiniteNonNegative(glm::vec3 value) noexcept
		{
			return IsFinite(value) && value.x >= 0.0f && value.y >= 0.0f && value.z >= 0.0f;
		}

		[[nodiscard]] bool IsFiniteInRange(float value, float minimum, float maximum) noexcept
		{
			return std::isfinite(value) && value >= minimum && value <= maximum;
		}
	}

	void ValidateVoxelRenderStyle(
		const ve::rendering::VoxelRenderStyle& style,
		std::vector<std::string>& issues)
	{
		const float largest_sun_direction_component = std::max(
			std::abs(style.sun_direction.x),
			std::max(std::abs(style.sun_direction.y), std::abs(style.sun_direction.z)));
		if (!IsFinite(style.sun_direction) || largest_sun_direction_component <= 0.000001f)
			issues.push_back("voxel_render_style.sun_direction must be finite and non-zero");
		if (!IsFiniteNonNegative(style.sun_color) ||
			!IsFiniteNonNegative(style.sky_horizon_color) ||
			!IsFiniteNonNegative(style.sky_zenith_color))
			issues.push_back("voxel_render_style colors must contain finite non-negative values");
		if (!std::isfinite(style.sun_intensity) || style.sun_intensity < 0.0f)
			issues.push_back("voxel_render_style.sun_intensity must be finite and non-negative");
		if (!std::isfinite(style.exposure) || style.exposure <= 0.0f)
			issues.push_back("voxel_render_style.exposure must be finite and greater than zero");
		if (!std::isfinite(style.fog_start_distance) || style.fog_start_distance < 0.0f)
			issues.push_back("voxel_render_style.fog_start_distance must be finite and non-negative");
		if (!std::isfinite(style.fog_end_distance) ||
			style.fog_end_distance <= style.fog_start_distance)
			issues.push_back("voxel_render_style.fog_end_distance must be finite and greater than fog_start_distance");
		if (!IsFiniteInRange(style.fog_strength, 0.0f, 1.0f) ||
			!IsFiniteInRange(style.cloud_coverage, 0.0f, 1.0f) ||
			!IsFiniteInRange(style.cloud_density, 0.0f, 1.0f))
			issues.push_back("voxel_render_style atmosphere strengths must be finite and between 0 and 1");
		if (!std::isfinite(style.cloud_speed) || style.cloud_speed < 0.0f)
			issues.push_back("voxel_render_style.cloud_speed must be finite and non-negative");
		ValidateVoxelSurfaceStyle(style, issues);
	}
}
