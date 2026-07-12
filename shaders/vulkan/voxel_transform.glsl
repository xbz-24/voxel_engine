#ifndef VE_VOXEL_TRANSFORM_GLSL
#define VE_VOXEL_TRANSFORM_GLSL

layout(push_constant) uniform VoxelTransformPushConstants
{
	mat4 transform;
} voxel_transform;

#endif
