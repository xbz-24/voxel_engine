#ifndef VE_VOXEL_DETAIL_FILTER_GLSL
#define VE_VOXEL_DETAIL_FILTER_GLSL

#include "voxel_environment.glsl"

float voxel_procedural_detail_visibility(
	vec3 world_position,
	vec2 surface_coordinate,
	VoxelEnvironment environment)
{
	vec2 coordinate_footprint = fwidth(surface_coordinate);
	float largest_footprint = max(coordinate_footprint.x, coordinate_footprint.y);
	float derivative_visibility = 1.0 - smoothstep(0.18, 0.82, largest_footprint);
	float camera_distance = length(environment.camera_position - world_position);
	float distance_visibility = 1.0 - smoothstep(96.0, 360.0, camera_distance);
	return derivative_visibility * distance_visibility;
}

#endif
