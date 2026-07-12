#ifndef VE_VOXEL_SKY_SCATTERING_GLSL
#define VE_VOXEL_SKY_SCATTERING_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"

float voxel_rayleigh_phase(float cosine)
{
	return 0.0596831 * (1.0 + cosine * cosine);
}

float voxel_mie_phase(float cosine, float anisotropy)
{
	float anisotropy_squared = anisotropy * anisotropy;
	float denominator = pow(max(1.0 + anisotropy_squared - 2.0 * anisotropy * cosine, 0.001), 1.5);
	return 0.1193662 * (1.0 - anisotropy_squared) * (1.0 + cosine * cosine) /
		((2.0 + anisotropy_squared) * denominator);
}

vec3 evaluate_voxel_sky_scattering(vec3 view_direction, VoxelEnvironment environment)
{
	float elevation = view_direction.y;
	float zenith_mix = smoothstep(-0.08, 0.90, elevation);
	float horizon_depth = pow(1.0 - saturate(abs(elevation)), 4.0);
	float sun_alignment = dot(view_direction, environment.sun_direction);
	vec3 base_sky = mix(environment.sky_horizon_color, environment.sky_zenith_color, zenith_mix);
	vec3 rayleigh_color = environment.sky_zenith_color *
		voxel_rayleigh_phase(sun_alignment) * (0.85 + zenith_mix * 0.35);
	vec3 mie_color = environment.sun_color *
		voxel_mie_phase(sun_alignment, 0.78) * environment.sun_intensity * 0.34;
	vec3 horizon_scatter = environment.sky_horizon_color * horizon_depth * 0.22;
	return max(base_sky + rayleigh_color + mie_color + horizon_scatter, vec3(0.0));
}

#endif
