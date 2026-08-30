#ifndef VE_VOXEL_SPECULAR_GLSL
#define VE_VOXEL_SPECULAR_GLSL

#include "voxel_brdf.glsl"

vec3 evaluate_voxel_specular(
	vec3 albedo,
	vec3 normal,
	vec3 view_direction,
	float roughness,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	vec3 ggx = evaluate_voxel_ggx_specular(
		albedo,
		normal,
		view_direction,
		roughness,
		masks,
		environment);
	float snow_glint = pow(max(dot(reflect(-environment.sun_direction, normal), view_direction), 0.0), 92.0);
	return ggx + environment.sun_color * snow_glint * masks.snow * 0.08 * environment.specular_strength;
}

#endif
