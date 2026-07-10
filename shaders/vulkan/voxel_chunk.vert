#version 450

#extension GL_GOOGLE_include_directive : require

#include "voxel_environment.glsl"
#include "voxel_vertex.glsl"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in float in_light;
layout(location = 3) in vec4 in_normal;

layout(location = 0) out vec4 frag_color;
layout(location = 1) out float frag_light;
layout(location = 2) out vec3 frag_normal;
layout(location = 3) out float frag_view_depth;
layout(location = 4) out vec3 frag_world_position;

void main()
{
	vec4 clip_position = voxel_push_constants.transform * vec4(in_position, 1.0);
	gl_Position = clip_position;
	frag_color = in_color;
	frag_light = in_light;
	frag_normal = normalized_voxel_normal(in_normal.xyz);
	frag_view_depth = voxel_view_depth(clip_position);
	frag_world_position = in_position;
}
