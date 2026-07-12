#ifndef VE_VOXEL_LIGHTING_GLSL
#define VE_VOXEL_LIGHTING_GLSL

#include "voxel_emission.glsl"
#include "voxel_environment.glsl"
#include "voxel_reflections.glsl"
#include "voxel_shadowing.glsl"
#include "voxel_specular.glsl"

vec3 evaluate_voxel_lighting(vec3 albedo,
	vec3 normal,
	vec3 view_direction,
	float vertex_light,
	float height_blend,
	vec3 world_position,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	vec3 horizon_fill_direction = normalize(vec3(0.52, 0.18, 0.72));

	float direct_light = max(dot(normal, environment.sun_direction), 0.0);
	float horizon_light = saturate(dot(normal, horizon_fill_direction) * 0.5 + 0.5);
	float sky_bounce = saturate((normal.y * 0.5) + 0.5);
	float side_fill = 1.0 - abs(normal.y);
	float grazing_rim = pow(saturate(1.0 - direct_light), 2.2) * side_fill;

	vec3 cool_shadow = mix(environment.sky_horizon_color, environment.sky_zenith_color, 0.72);
	vec3 warm_sun = environment.sun_color;
	vec3 high_sky = environment.sky_zenith_color * 1.22;
	vec3 low_bounce = environment.sky_horizon_color * vec3(1.18, 1.04, 0.90);
	vec3 light_color = mix(cool_shadow, warm_sun, smoothstep(0.05, 0.95, direct_light));
	light_color = mix(light_color, high_sky, sky_bounce * height_blend * 0.18);
	light_color = mix(light_color, low_bounce, horizon_light * (1.0 - height_blend) * 0.08);

	float ambient = mix(0.34, 0.64, sky_bounce);
	float diffuse = 0.14 + direct_light * 0.62 * environment.sun_intensity;
	float rim = 0.075 * grazing_rim * sky_bounce;
	float contact = mix(0.94, 1.0, sky_bounce) *
		voxel_micro_shadow(world_position, normal) *
		voxel_contact_shadow(world_position, normal, masks, environment) *
		voxel_horizon_occlusion(normal, height_blend);
	float clamped_vertex_light = clamp(vertex_light, 0.0, 1.70);
	float roughness = voxel_surface_roughness(world_position, normal, masks);
	vec3 specular = evaluate_voxel_specular(
		albedo,
		normal,
		view_direction,
		roughness,
		masks,
		environment);

	vec3 lit = albedo * clamped_vertex_light * (ambient + diffuse + rim) * light_color * contact;
	lit += vec3(0.015, 0.020, 0.030) * side_fill * (1.0 - direct_light);
	lit += specular;
	lit = apply_voxel_reflections(lit, normal, view_direction, masks, environment);
	lit = apply_voxel_emission(lit, albedo, world_position, masks, environment);
	return max(lit - vec3(0.014), vec3(0.0));
}

#endif
