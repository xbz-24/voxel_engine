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
	};
}
