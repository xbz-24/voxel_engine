#ifndef VE_VOXEL_TONEMAP_GLSL
#define VE_VOXEL_TONEMAP_GLSL

#include "voxel_math.glsl"

vec3 voxel_filmic_tonemap(vec3 color, float exposure)
{
	vec3 exposed = max(color * max(exposure, 0.001), vec3(0.0));
	return saturate((exposed * (2.51 * exposed + 0.03)) /
		(exposed * (2.43 * exposed + 0.59) + 0.14));
}

#endif
