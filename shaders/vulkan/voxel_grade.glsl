#ifndef VE_VOXEL_GRADE_GLSL
#define VE_VOXEL_GRADE_GLSL

#include "voxel_math.glsl"
#include "voxel_surface_masks.glsl"

vec3 apply_voxel_color_grade(vec3 color, vec3 world_position, VoxelSurfaceMasks masks)
{
	float warmth = saturate((1.0 - masks.height) * 0.45 + masks.dryness * 0.25);
	float cool_air = saturate(masks.height * 0.35 + masks.snow * 0.18);
	vec3 graded = color;
	graded *= mix(vec3(1.0), vec3(1.035, 0.995, 0.940), warmth);
	graded *= mix(vec3(1.0), vec3(0.945, 0.990, 1.070), cool_air);
	graded = mix(graded, graded * vec3(0.84, 0.95, 1.12), masks.water * 0.22);
	graded += vec3(masks.light_source * 0.035);

	float luminance = dot(graded, vec3(0.2126, 0.7152, 0.0722));
	graded = mix(vec3(luminance), graded, 1.06 + masks.greenery * 0.04);
	float exposure_jitter = (hash13(floor(world_position * vec3(0.05, 0.13, 0.05))) - 0.5) * 0.018;
	return saturate(graded + vec3(exposure_jitter));
}

#endif
