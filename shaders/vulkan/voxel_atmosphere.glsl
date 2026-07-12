#ifndef VE_VOXEL_ATMOSPHERE_GLSL
#define VE_VOXEL_ATMOSPHERE_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_tonemap.glsl"

vec3 apply_voxel_atmosphere(
	vec3 lit_color,
	float view_depth,
	vec3 world_position,
	float height_blend,
	VoxelEnvironment environment)
{
	vec3 low_fog_color = mix(
		environment.sky_horizon_color * vec3(1.00, 0.98, 0.94),
		environment.sky_horizon_color * vec3(0.96, 1.02, 1.08),
		smoothstep(45.0, 80.0, world_position.y));
	vec3 high_fog_color = environment.sky_zenith_color;
	vec3 fog_color = mix(low_fog_color, high_fog_color, height_blend);
	float distance_fog = smoothstep(
		environment.fog_start_distance,
		environment.fog_end_distance,
		view_depth);
	float ground_haze = smoothstep(33.0, 52.0, world_position.y) *
		(1.0 - smoothstep(74.0, 128.0, world_position.y));
	float valley_haze = ground_haze * (1.0 - height_blend) * 0.08;
	float far_blue_shift = smoothstep(280.0, 620.0, view_depth) * 0.09;

	vec3 graded = voxel_filmic_tonemap(lit_color, environment.exposure);
	float luminance = dot(graded, vec3(0.2126, 0.7152, 0.0722));
	graded = mix(vec3(luminance), graded, 1.10);
	graded = pow(saturate(graded), vec3(0.92));
	vec3 aerial_tint = mix(vec3(1.0), environment.sky_zenith_color + vec3(0.35), 0.12);
	graded = mix(graded, graded * aerial_tint, far_blue_shift);
	float fog_amount = saturate(distance_fog * environment.fog_strength + valley_haze * environment.fog_strength);
	return mix(graded, fog_color, fog_amount);
}

#endif
