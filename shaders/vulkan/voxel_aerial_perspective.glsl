#ifndef VE_VOXEL_AERIAL_PERSPECTIVE_GLSL
#define VE_VOXEL_AERIAL_PERSPECTIVE_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"

struct VoxelAerialPerspective
{
	vec3 inscattering;
	float transmittance;
};

VoxelAerialPerspective evaluate_voxel_aerial_perspective(
	float view_depth,
	vec3 world_position,
	float height_blend,
	VoxelEnvironment environment)
{
	vec3 camera_to_surface = world_position - environment.camera_position;
	float ray_length_squared = dot(camera_to_surface, camera_to_surface);
	vec3 view_direction = ray_length_squared > 0.000001
		? camera_to_surface * inversesqrt(ray_length_squared)
		: vec3(0.0, 0.0, -1.0);
	float distance_fog = smoothstep(
		environment.fog_start_distance,
		environment.fog_end_distance,
		view_depth);
	float midpoint_height = (environment.camera_position.y + world_position.y) * 0.5;
	float height_density = mix(1.18, 0.62, smoothstep(32.0, 168.0, midpoint_height));
	float horizon_air_mass = mix(1.24, 0.86, saturate(abs(view_direction.y)));
	float optical_depth = distance_fog * 1.55 * height_density * horizon_air_mass *
		environment.fog_strength * environment.aerial_perspective_strength;
	float transmittance = exp(-optical_depth);

	float sun_alignment = clamp(dot(view_direction, environment.sun_direction), -1.0, 1.0);
	float rayleigh_phase = 0.75 + 0.25 * sun_alignment * sun_alignment;
	float forward_scatter = pow(saturate(sun_alignment), 12.0);
	vec3 atmospheric_color = mix(
		environment.sky_horizon_color,
		environment.sky_zenith_color,
		saturate(height_blend * 0.72 + max(view_direction.y, 0.0) * 0.28));
	atmospheric_color *= rayleigh_phase;
	atmospheric_color += environment.sun_color * environment.sun_intensity * forward_scatter * 0.16;
	return VoxelAerialPerspective(max(atmospheric_color, vec3(0.0)), transmittance);
}

#endif
