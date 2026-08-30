#ifndef VE_VOXEL_CLOUD_LIGHTING_GLSL
#define VE_VOXEL_CLOUD_LIGHTING_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_noise.glsl"

float voxel_cloud_forward_phase(float cosine)
{
	const float anisotropy = 0.62;
	float anisotropy_squared = anisotropy * anisotropy;
	float denominator = pow(
		max(1.0 + anisotropy_squared - 2.0 * anisotropy * cosine, 0.001),
		1.5);
	return (1.0 - anisotropy_squared) / max(4.0 * 3.14159265 * denominator, 0.001);
}

vec4 evaluate_voxel_cloud_lighting(
	vec3 view_direction,
	vec2 cloud_coordinate,
	float density,
	float erosion,
	VoxelEnvironment environment)
{
	vec2 horizontal_sun = environment.sun_direction.xz;
	float horizontal_length_squared = dot(horizontal_sun, horizontal_sun);
	horizontal_sun = horizontal_length_squared > 0.000001
		? horizontal_sun * inversesqrt(horizontal_length_squared)
		: vec2(0.0, 1.0);
	float near_light_density = voxel_value_noise(cloud_coordinate * 1.71 + horizontal_sun * 0.46);
	float far_light_density = voxel_value_noise(cloud_coordinate * 1.71 + horizontal_sun * 0.92 + vec2(7.0, 3.0));
	float self_shadow_density = mix(near_light_density, far_light_density, 0.38);
	float optical_depth = density * (1.15 + self_shadow_density * 0.85) *
		environment.cloud_light_absorption * 1.75;
	float direct_transmittance = exp(-optical_depth);
	float powder = 1.0 - exp(-density * 2.6);
	float sun_alignment = clamp(dot(view_direction, environment.sun_direction), -1.0, 1.0);
	float silver_lining = saturate(voxel_cloud_forward_phase(sun_alignment) * 1.65) *
		powder * (0.45 + erosion * 0.55);

	vec3 ambient_color = mix(environment.sky_horizon_color, vec3(0.50, 0.55, 0.63), 0.48);
	vec3 direct_color = mix(vec3(0.86, 0.90, 0.96), environment.sun_color, 0.28);
	vec3 cloud_color = ambient_color * (0.72 + powder * 0.18);
	cloud_color += direct_color * mix(0.18, 0.78, direct_transmittance);
	cloud_color += environment.sun_color * environment.sun_intensity * silver_lining * 0.32;
	float alpha = (1.0 - exp(-density * 2.25)) * smoothstep(0.0, 0.08, density);
	return vec4(max(cloud_color, vec3(0.0)), saturate(alpha));
}

#endif
