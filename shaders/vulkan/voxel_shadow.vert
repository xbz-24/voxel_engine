#version 450

#extension GL_GOOGLE_include_directive : require

#include "voxel_environment.glsl"

layout(location = 0) in vec3 in_position;

void main()
{
	gl_Position = voxel_frame.light_view_projection * vec4(in_position, 1.0);
}
