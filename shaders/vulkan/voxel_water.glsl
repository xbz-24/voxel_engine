#ifndef VE_VOXEL_WATER_GLSL
#define VE_VOXEL_WATER_GLSL

#include "voxel_brdf.glsl"
#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_surface_masks.glsl"

vec3 apply_voxel_water_reflection(
	vec3 lit_color,
	vec3 normal,
	vec3 view_direction,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	vec3 reflection_direction = reflect(-view_direction, normal);
	float reflected_elevation = saturate(reflection_direction.y * 0.5 + 0.5);
	vec3 reflected_sky = mix(
		environment.sky_horizon_color,
		environment.sky_zenith_color,
		reflected_elevation);
	float normal_view = max(dot(normal, view_direction), 0.0);
	vec3 fresnel = voxel_fresnel_schlick(normal_view, vec3(0.020));
	float sun_glint = pow(max(dot(reflection_direction, environment.sun_direction), 0.0), 196.0);
	vec3 reflected = reflected_sky * (0.55 + fresnel * 0.85) +
		environment.sun_color * environment.sun_intensity * sun_glint * 1.8;
	float reflection_amount = saturate(
		masks.water * environment.water_reflection_strength *
		(0.24 + dot(fresnel, vec3(0.3333)) * 0.76));
	return mix(lit_color, lit_color * 0.68 + reflected, reflection_amount);
}

#endif
