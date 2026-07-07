#version 450

layout(push_constant) uniform PushConstants
{
	mat4 mvp;
} pc;

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
	vec4 clip_position = pc.mvp * vec4(in_position, 1.0);
	gl_Position = clip_position;
	frag_color = in_color;
	frag_light = in_light;
	frag_normal = normalize(in_normal.xyz);
	frag_view_depth = max(clip_position.w, 0.0);
	frag_world_position = in_position;
}
