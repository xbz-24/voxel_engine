#ifndef VE_VOXEL_AMBIENT_OCCLUSION_GLSL
#define VE_VOXEL_AMBIENT_OCCLUSION_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_surface_detail.glsl"
#include "voxel_surface_masks.glsl"

float evaluate_voxel_ambient_occlusion(
	vec3 world_position,
	vec3 normal,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	float edge = voxel_edge_mask(world_position, normal);
	float corner = voxel_corner_mask(world_position, normal);
	float vertical_face = 1.0 - abs(normal.y);
	float underside = saturate(-normal.y);
	float material_cavity = saturate(masks.stone + masks.dryness * 0.55 + masks.greenery * 0.28);
	float cavity = corner * 0.22 + edge * vertical_face * 0.10 + underside * 0.08;
	cavity *= mix(0.55, 1.0, material_cavity);
	cavity *= 1.0 - masks.water * 0.82;
	cavity *= 1.0 - masks.light_source * 0.94;
	float camera_distance = length(environment.camera_position - world_position);
	float distance_visibility = 1.0 - smoothstep(120.0, 420.0, camera_distance);
	return saturate(1.0 - cavity * environment.ambient_occlusion_strength * distance_visibility);
}

#endif
