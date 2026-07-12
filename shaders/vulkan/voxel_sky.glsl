#ifndef VE_VOXEL_SKY_GLSL
#define VE_VOXEL_SKY_GLSL

#include "voxel_clouds.glsl"
#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_sky_scattering.glsl"
#include "voxel_tonemap.glsl"

vec3 evaluate_voxel_sky(vec3 view_direction, VoxelEnvironment environment)
{
	vec3 direction = normalize(view_direction);
	float elevation = direction.y;
	vec3 sky_color = evaluate_voxel_sky_scattering(direction, environment);

	float below_horizon = 1.0 - smoothstep(-0.30, 0.05, elevation);
	vec3 ground_scatter = environment.sky_horizon_color * vec3(0.34, 0.38, 0.42);
	sky_color = mix(sky_color, ground_scatter, below_horizon);

	float sun_alignment = dot(direction, environment.sun_direction);
	float sun_disc = smoothstep(0.99925, 0.99982, sun_alignment);
	float sun_halo = pow(saturate(sun_alignment), 96.0) * 0.34;
	float broad_glow = pow(saturate(sun_alignment), 12.0) * 0.08;
	vec3 sun_radiance = environment.sun_color * environment.sun_intensity;
	vec4 clouds = evaluate_voxel_clouds(direction, environment);
	sky_color = mix(sky_color, clouds.rgb, clouds.a);
	sky_color += sun_radiance * (sun_disc * 3.2 + sun_halo + broad_glow) * (1.0 - clouds.a * 0.82);
	return voxel_filmic_tonemap(sky_color, environment.exposure);
}

#endif
