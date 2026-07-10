#ifndef VE_VOXEL_REFLECTIONS_GLSL
#define VE_VOXEL_REFLECTIONS_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_surface_masks.glsl"

vec3 apply_voxel_reflections(
	vec3 lit_color,
	vec3 normal,
	vec3 world_position,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	float sky_visibility = saturate(normal.y * 0.5 + 0.5);
	vec3 reflected_sky = mix(environment.sky_horizon_color, environment.sky_zenith_color, sky_visibility);
	float ripple = hash13(floor(world_position * vec3(0.41, 2.70, 0.41))) - 0.5;
	float water_reflection = masks.water * (0.30 + sky_visibility * 0.26 + ripple * 0.05);
	vec3 reflected_water = lit_color * 0.74 + reflected_sky * (0.30 + environment.sun_intensity * 0.04);
	return mix(lit_color, reflected_water, saturate(water_reflection));
}

#endif
