#ifndef VE_VOXEL_MATERIALS_GLSL
#define VE_VOXEL_MATERIALS_GLSL

#include "voxel_surface_detail.glsl"

vec3 apply_voxel_material_tint(vec3 albedo,
	vec3 world_position,
	vec3 normal,
	float height_blend,
	VoxelSurfaceMasks masks)
{
	float strata = (hash13(floor(world_position * vec3(0.09, 0.23, 0.09))) - 0.5) * masks.vertical;
	vec3 tinted = albedo;
	tinted *= mix(vec3(1.0), vec3(0.92, 1.04, 0.96), masks.greenery * 0.20);
	tinted *= mix(vec3(1.0), vec3(1.05, 0.98, 0.89), masks.dryness * (1.0 - height_blend) * 0.16);
	tinted *= mix(vec3(1.0), vec3(0.92, 0.98, 1.08), masks.snow * height_blend * 0.18);
	tinted *= mix(vec3(1.0), vec3(0.82, 0.94, 1.16), masks.water * 0.24);
	tinted += vec3(strata * 0.035 * (1.0 - masks.water));
	tinted = apply_voxel_surface_detail(tinted, world_position, normal, masks);
	return max(tinted, vec3(0.0));
}

#endif
