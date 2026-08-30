#ifndef VE_VOXEL_REFLECTIONS_GLSL
#define VE_VOXEL_REFLECTIONS_GLSL

#include "voxel_water.glsl"

vec3 apply_voxel_reflections(
	vec3 lit_color,
	vec3 normal,
	vec3 view_direction,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	vec3 reflected = apply_voxel_water_reflection(
		lit_color,
		normal,
		view_direction,
		masks,
		environment);
	float snow_sky_visibility = masks.snow * saturate(normal.y) * 0.08;
	return mix(reflected, reflected + environment.sky_zenith_color * 0.06, snow_sky_visibility);
}

#endif
