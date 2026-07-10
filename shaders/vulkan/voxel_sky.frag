#version 450

#extension GL_GOOGLE_include_directive : require

#include "voxel_environment.glsl"
#include "voxel_sky.glsl"

layout(location = 0) in vec2 frag_ndc;

layout(location = 0) out vec4 out_color;

void main()
{
	vec4 unprojected = voxel_push_constants.transform * vec4(frag_ndc, 1.0, 1.0);
	float reciprocal_w = abs(unprojected.w) > 0.000001 ? 1.0 / unprojected.w : 1.0;
	vec3 view_direction = normalize(unprojected.xyz * reciprocal_w);
	VoxelEnvironment environment = current_voxel_environment();
	out_color = vec4(evaluate_voxel_sky(view_direction, environment), 1.0);
}
