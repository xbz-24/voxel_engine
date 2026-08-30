#ifndef VE_VOXEL_VERTEX_GLSL
#define VE_VOXEL_VERTEX_GLSL

vec3 normalized_voxel_normal(vec3 packed_normal)
{
	float length_squared = dot(packed_normal, packed_normal);
	return length_squared > 0.0001 ? packed_normal * inversesqrt(length_squared) : vec3(0.0, 1.0, 0.0);
}

float voxel_view_depth(vec4 clip_position)
{
	return max(clip_position.w, 0.0);
}

#endif
