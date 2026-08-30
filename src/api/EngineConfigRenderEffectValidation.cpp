#include "EngineConfigValidationInternal.h"

#include <cmath>
#include <string>

namespace voxel::detail::config_validation
{
	void ValidateVoxelRenderEffects(const VoxelRenderStyle& style,
		std::vector<std::string>& issues)
	{
		const auto validate_normalized = [&issues](float value, const char* label)
		{
			if (!std::isfinite(value) || value < 0.0f || value > 1.0f)
			{
				issues.push_back(std::string{ label } + " must be finite and between 0 and 1");
			}
		};
		const auto validate_response_strength = [&issues](float value, const char* label)
		{
			if (!std::isfinite(value) || value < 0.0f || value > 2.0f)
			{
				issues.push_back(std::string{ label } + " must be finite and between 0 and 2");
			}
		};

		validate_normalized(style.fog_strength, "voxel_render_style.fog_strength");
		validate_normalized(style.cloud_coverage, "voxel_render_style.cloud_coverage");
		validate_normalized(style.cloud_density, "voxel_render_style.cloud_density");
		if (!std::isfinite(style.cloud_speed) || style.cloud_speed < 0.0f)
		{
			issues.push_back("voxel_render_style.cloud_speed must be finite and non-negative");
		}
		validate_response_strength(style.surface_detail_strength, "voxel_render_style.surface_detail_strength");
		validate_response_strength(style.water_reflection_strength, "voxel_render_style.water_reflection_strength");
		validate_response_strength(style.shadow_strength, "voxel_render_style.shadow_strength");
		validate_response_strength(style.specular_strength, "voxel_render_style.specular_strength");
		validate_response_strength(style.ambient_occlusion_strength, "voxel_render_style.ambient_occlusion_strength");
		validate_response_strength(style.aerial_perspective_strength, "voxel_render_style.aerial_perspective_strength");
		validate_response_strength(style.subsurface_scattering_strength, "voxel_render_style.subsurface_scattering_strength");
		validate_response_strength(style.cloud_light_absorption, "voxel_render_style.cloud_light_absorption");
		validate_response_strength(style.directional_shadow_strength, "voxel_render_style.directional_shadow_strength");
		validate_response_strength(style.directional_shadow_softness, "voxel_render_style.directional_shadow_softness");
		if (!std::isfinite(style.directional_shadow_distance) ||
			style.directional_shadow_distance < 32.0f || style.directional_shadow_distance > 512.0f)
		{
			issues.push_back("voxel_render_style.directional_shadow_distance must be finite and between 32 and 512");
		}
	}
}
