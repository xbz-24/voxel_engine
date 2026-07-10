#ifndef VE_VOXEL_EMISSION_GLSL
#define VE_VOXEL_EMISSION_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_surface_masks.glsl"

vec3 apply_voxel_emission(
	vec3 lit_color,
	vec3 albedo,
	vec3 world_position,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	float cell_variation = mix(0.92, 1.08, hash13(floor(world_position * 0.63)));
	vec3 emission_color = mix(albedo, environment.sun_color, 0.18);
	return lit_color + emission_color * masks.light_source * 0.34 * cell_variation;
}

#endif
