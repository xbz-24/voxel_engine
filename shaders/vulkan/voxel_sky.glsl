#ifndef VE_VOXEL_SKY_GLSL
#define VE_VOXEL_SKY_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_tonemap.glsl"

vec3 evaluate_voxel_sky(vec3 view_direction, VoxelEnvironment environment)
{
	vec3 direction = normalize(view_direction);
	float elevation = direction.y;
	float zenith_mix = smoothstep(-0.08, 0.88, elevation);
	float horizon_band = pow(1.0 - saturate(abs(elevation)), 5.0);
	vec3 sky_color = mix(environment.sky_horizon_color, environment.sky_zenith_color, zenith_mix);
	sky_color += environment.sky_horizon_color * horizon_band * 0.18;

	float below_horizon = 1.0 - smoothstep(-0.30, 0.05, elevation);
	vec3 ground_scatter = environment.sky_horizon_color * vec3(0.34, 0.38, 0.42);
	sky_color = mix(sky_color, ground_scatter, below_horizon);

	float sun_alignment = dot(direction, environment.sun_direction);
	float sun_disc = smoothstep(0.99925, 0.99982, sun_alignment);
	float sun_halo = pow(saturate(sun_alignment), 96.0) * 0.34;
	float broad_glow = pow(saturate(sun_alignment), 12.0) * 0.08;
	vec3 sun_radiance = environment.sun_color * environment.sun_intensity;
	sky_color += sun_radiance * (sun_disc * 3.2 + sun_halo + broad_glow);
	return voxel_filmic_tonemap(sky_color, environment.exposure);
}

#endif
