#ifndef VE_VOXEL_SHADOWING_GLSL
#define VE_VOXEL_SHADOWING_GLSL

#include "voxel_math.glsl"
#include "voxel_surface_detail.glsl"

float voxel_micro_shadow(vec3 world_position, vec3 normal)
{
	float vertical_face = 1.0 - abs(normal.y);
	float cell_noise = hash13(floor(world_position * 1.37));
	float fine_noise = hash13(floor(world_position * 3.71 + vec3(17.0, 3.0, 11.0)));
	return mix(0.965, 1.035, cell_noise) * mix(0.985, 1.015, fine_noise) * mix(1.0, 0.985, vertical_face);
}

float voxel_contact_shadow(vec3 world_position, vec3 normal, VoxelSurfaceMasks masks)
{
	float edge = voxel_edge_mask(world_position, normal);
	float corner = voxel_corner_mask(world_position, normal);
	float underside = saturate(-normal.y);
	float vertical_crevice = masks.vertical * edge;
	float material_strength = mix(0.18, 0.34, masks.stone + masks.dryness * 0.35);
	material_strength = mix(material_strength, 0.06, masks.water);
	material_strength = mix(material_strength, 0.02, masks.light_source);
	float shadow = (vertical_crevice * material_strength) + (corner * 0.10) + (underside * 0.12);
	return saturate(1.0 - shadow);
}

float voxel_horizon_occlusion(vec3 normal, float height_blend)
{
	float low_face_occlusion = (1.0 - saturate(normal.y * 0.5 + 0.5)) * 0.10;
	float high_air_lift = height_blend * saturate(normal.y) * 0.05;
	return saturate(1.0 - low_face_occlusion + high_air_lift);
}

#endif
