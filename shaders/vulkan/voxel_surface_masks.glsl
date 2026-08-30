#ifndef VE_VOXEL_SURFACE_MASKS_GLSL
#define VE_VOXEL_SURFACE_MASKS_GLSL

#include "voxel_math.glsl"

struct VoxelSurfaceMasks
{
	float greenery;
	float dryness;
	float stone;
	float snow;
	float water;
	float light_source;
	float upward;
	float vertical;
	float height;
};

VoxelSurfaceMasks build_voxel_surface_masks(vec4 color, vec3 normal, vec3 world_position, float height_blend)
{
	float brightest_channel = max(color.r, max(color.g, color.b));
	float darkest_channel = min(color.r, min(color.g, color.b));
	float color_range = brightest_channel - darkest_channel;
	VoxelSurfaceMasks masks;
	masks.greenery = saturate((color.g - max(color.r, color.b)) * 3.6);
	masks.dryness = saturate(((color.r * 0.85) + (color.g * 0.52) - (color.b * 1.12) - 0.10) * 1.55);
	masks.stone = saturate((1.0 - color_range * 3.4) * smoothstep(0.18, 0.72, brightest_channel));
	masks.snow = saturate((color.r + color.g + color.b - 2.05) * 1.30 + height_blend * 0.25);
	masks.water = saturate((color.b - max(color.r, color.g) * 0.72) * 2.4 + (0.86 - color.a) * 1.6);
	masks.light_source = saturate((brightest_channel - 0.82) * 3.2) *
		saturate((color.g + color.b) * 0.55) *
		(1.0 - masks.snow * 0.55) *
		(1.0 - masks.water * 0.45);
	masks.upward = saturate(normal.y);
	masks.vertical = 1.0 - abs(normal.y);
	masks.height = height_blend;

	float terrace_noise = hash13(floor(world_position * vec3(0.08, 0.21, 0.08)));
	masks.dryness = saturate(masks.dryness + (terrace_noise - 0.5) * 0.10);
	masks.greenery = saturate(masks.greenery - masks.dryness * 0.22 + masks.upward * 0.05);
	return masks;
}

#endif
