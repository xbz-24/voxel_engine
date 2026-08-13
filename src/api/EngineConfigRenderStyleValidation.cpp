#include "EngineConfigValidationInternal.h"

#include <algorithm>
#include <cmath>

namespace voxel::detail::config_validation
{
	namespace
	{
		[[nodiscard]] bool IsFiniteVector(Vec3 value) noexcept
		{
			return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
		}

		[[nodiscard]] bool IsFiniteNonNegativeVector(Vec3 value) noexcept
		{
			return IsFiniteVector(value) && value.x >= 0.0f && value.y >= 0.0f && value.z >= 0.0f;
		}
	}

	void ValidateVoxelRenderLightingAndFog(const VoxelRenderStyle& style,
		std::vector<std::string>& issues)
	{
		const float largest_direction_component = std::max(
			std::abs(style.sun_direction.x),
			std::max(std::abs(style.sun_direction.y), std::abs(style.sun_direction.z)));
		if (!IsFiniteVector(style.sun_direction) || largest_direction_component <= 0.000001f)
		{
			issues.push_back("voxel_render_style.sun_direction must be finite and non-zero");
		}
		if (!IsFiniteNonNegativeVector(style.sun_color))
		{
			issues.push_back("voxel_render_style.sun_color must contain finite non-negative values");
		}
		if (!IsFiniteNonNegativeVector(style.sky_horizon_color))
		{
			issues.push_back("voxel_render_style.sky_horizon_color must contain finite non-negative values");
		}
		if (!IsFiniteNonNegativeVector(style.sky_zenith_color))
		{
			issues.push_back("voxel_render_style.sky_zenith_color must contain finite non-negative values");
		}
		if (!std::isfinite(style.sun_intensity) || style.sun_intensity < 0.0f)
		{
			issues.push_back("voxel_render_style.sun_intensity must be finite and non-negative");
		}
		if (!std::isfinite(style.exposure) || style.exposure <= 0.0f)
		{
			issues.push_back("voxel_render_style.exposure must be finite and greater than zero");
		}
		if (!std::isfinite(style.fog_start_distance) || style.fog_start_distance < 0.0f)
		{
			issues.push_back("voxel_render_style.fog_start_distance must be finite and non-negative");
		}
		if (!std::isfinite(style.fog_end_distance) ||
			style.fog_end_distance <= style.fog_start_distance)
		{
			issues.push_back("voxel_render_style.fog_end_distance must be finite and greater than fog_start_distance");
		}
	}
}
