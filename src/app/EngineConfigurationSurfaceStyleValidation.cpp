#include "EngineConfigurationValidationDetail.h"
#include "VoxelRenderStyle.h"

#include <cmath>

namespace ve::engine::configuration_validation
{
	namespace
	{
		[[nodiscard]] bool IsFiniteInRange(float value, float minimum, float maximum) noexcept
		{
			return std::isfinite(value) && value >= minimum && value <= maximum;
		}
	}

	void ValidateVoxelSurfaceStyle(
		const ve::rendering::VoxelRenderStyle& style,
		std::vector<std::string>& issues)
	{
		if (!IsFiniteInRange(style.surface_detail_strength, 0.0f, 2.0f) ||
			!IsFiniteInRange(style.water_reflection_strength, 0.0f, 2.0f) ||
			!IsFiniteInRange(style.shadow_strength, 0.0f, 2.0f) ||
			!IsFiniteInRange(style.specular_strength, 0.0f, 2.0f) ||
			!IsFiniteInRange(style.ambient_occlusion_strength, 0.0f, 2.0f) ||
			!IsFiniteInRange(style.aerial_perspective_strength, 0.0f, 2.0f) ||
			!IsFiniteInRange(style.subsurface_scattering_strength, 0.0f, 2.0f) ||
			!IsFiniteInRange(style.cloud_light_absorption, 0.0f, 2.0f) ||
			!IsFiniteInRange(style.directional_shadow_strength, 0.0f, 2.0f) ||
			!IsFiniteInRange(style.directional_shadow_softness, 0.0f, 2.0f))
		{
			issues.push_back("voxel_render_style surface strengths must be finite and between 0 and 2");
		}
		if (!IsFiniteInRange(style.directional_shadow_distance, 32.0f, 512.0f))
		{
			issues.push_back(
				"voxel_render_style.directional_shadow_distance must be finite and between 32 and 512");
		}
	}
}
