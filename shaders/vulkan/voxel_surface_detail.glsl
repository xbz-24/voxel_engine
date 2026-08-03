#ifndef VE_VOXEL_SURFACE_DETAIL_GLSL
#define VE_VOXEL_SURFACE_DETAIL_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_surface_masks.glsl"

vec2 voxel_face_uv(vec3 world_position, vec3 normal)
{
	vec3 absolute_normal = abs(normal);
	if (absolute_normal.y >= absolute_normal.x && absolute_normal.y >= absolute_normal.z)
	{
		return world_position.xz;
	}
	if (absolute_normal.x >= absolute_normal.z)
	{
		return world_position.zy;
	}
	return world_position.xy;
}

float voxel_edge_mask(vec3 world_position, vec3 normal)
{
	vec2 face_uv = voxel_face_uv(world_position, normal);
	vec2 local_uv = fract(face_uv);
	vec2 edge_distance = min(local_uv, 1.0 - local_uv);
	float nearest_edge = min(edge_distance.x, edge_distance.y);
	vec2 coordinate_footprint = fwidth(face_uv);
	float largest_footprint = max(coordinate_footprint.x, coordinate_footprint.y);
	float filter_width = clamp(largest_footprint * 0.65, 0.001, 0.075);
	float detail_visibility = 1.0 - smoothstep(0.16, 0.70, largest_footprint);
	return (1.0 - smoothstep(
		max(0.025 - filter_width, 0.0),
		0.115 + filter_width,
		nearest_edge)) * detail_visibility;
}

float voxel_corner_mask(vec3 world_position, vec3 normal)
{
	vec2 face_uv = voxel_face_uv(world_position, normal);
	vec2 local_uv = fract(face_uv);
	vec2 edge_distance = min(local_uv, 1.0 - local_uv);
	vec2 coordinate_footprint = fwidth(face_uv);
	float largest_footprint = max(coordinate_footprint.x, coordinate_footprint.y);
	float filter_width = clamp(largest_footprint * 0.65, 0.001, 0.075);
	float detail_visibility = 1.0 - smoothstep(0.16, 0.70, largest_footprint);
	vec2 corner_response = vec2(1.0) - smoothstep(
		vec2(max(0.020 - filter_width, 0.0)),
		vec2(0.120 + filter_width),
		edge_distance);
	return corner_response.x * corner_response.y * detail_visibility;
}

float layered_voxel_noise(vec3 world_position)
{
	float large_noise = hash13(floor(world_position * 0.31));
	float medium_noise = hash13(floor(world_position * 1.13 + vec3(13.0, 7.0, 3.0)));
	float fine_noise = hash13(floor(world_position * 3.91 + vec3(5.0, 19.0, 11.0)));
	return (large_noise * 0.50) + (medium_noise * 0.34) + (fine_noise * 0.16);
}

float voxel_surface_roughness(vec3 world_position, vec3 normal, VoxelSurfaceMasks masks)
{
	float noise = layered_voxel_noise(world_position);
	float base_roughness = mix(0.52, 0.88, masks.stone + masks.greenery * 0.45 + masks.dryness * 0.35);
	base_roughness = mix(base_roughness, 0.28, masks.water);
	base_roughness = mix(base_roughness, 0.22, masks.light_source * 0.35);
	return saturate(base_roughness + (noise - 0.5) * 0.16 + voxel_edge_mask(world_position, normal) * 0.05);
}

vec3 apply_voxel_surface_detail(
	vec3 albedo,
	vec3 world_position,
	vec3 normal,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	float noise = layered_voxel_noise(world_position);
	float edge = voxel_edge_mask(world_position, normal);
	float corner = voxel_corner_mask(world_position, normal);
	float slope_deposit = masks.vertical * (1.0 - masks.water) * (1.0 - masks.light_source);

	vec3 detailed = albedo;
	float detail_strength = environment.surface_detail_strength;
	detailed *= mix(1.0, mix(0.94, 1.06, noise), detail_strength);
	detailed = mix(detailed, detailed * vec3(0.72, 0.75, 0.79), edge * masks.stone * 0.24 * detail_strength);
	detailed = mix(detailed, detailed * vec3(0.80, 0.74, 0.64), edge * masks.dryness * 0.20 * detail_strength);
	detailed = mix(detailed, detailed * vec3(0.78, 0.88, 0.74), (1.0 - noise) * masks.greenery * slope_deposit * 0.18 * detail_strength);
	detailed = mix(detailed, detailed * vec3(1.05, 1.08, 1.12), masks.snow * masks.upward * 0.18 * detail_strength);
	detailed += vec3(corner * 0.025 * (masks.snow + masks.light_source) * detail_strength);
	return max(detailed, vec3(0.0));
}

#endif
