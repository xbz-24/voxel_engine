#pragma once

#include <glm/vec3.hpp>

namespace ve::rendering
{
	/** Runtime representation of backend-neutral voxel lighting and atmosphere settings. */
	struct VoxelRenderStyle
	{
		glm::vec3 sun_direction{ -0.42f, 0.78f, -0.46f };
		glm::vec3 sun_color{ 1.16f, 1.04f, 0.84f };
		glm::vec3 sky_horizon_color{ 0.72f, 0.70f, 0.64f };
		glm::vec3 sky_zenith_color{ 0.52f, 0.68f, 0.88f };
		float sun_intensity = 1.0f;
		float exposure = 1.0f;
		float fog_start_distance = 165.0f;
		float fog_end_distance = 455.0f;
		float fog_strength = 0.42f;
		float cloud_coverage = 0.42f;
		float cloud_density = 0.62f;
		float cloud_speed = 0.018f;
		float surface_detail_strength = 1.0f;
		float water_reflection_strength = 1.0f;
		float shadow_strength = 1.0f;
		float specular_strength = 1.0f;
		float ambient_occlusion_strength = 1.0f;
		float aerial_perspective_strength = 1.0f;
		float subsurface_scattering_strength = 1.0f;
		float cloud_light_absorption = 1.0f;
		float directional_shadow_strength = 0.85f;
		float directional_shadow_softness = 1.0f;
		float directional_shadow_distance = 220.0f;
	};
}
