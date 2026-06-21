#version 450

layout(push_constant) uniform PushConstants
{
	mat4 mvp;
} pc;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in float in_texture_layer;
layout(location = 3) in float in_light;

layout(location = 0) out vec2 frag_uv;
layout(location = 1) flat out int frag_texture_layer;
layout(location = 2) out float frag_light;

void main()
{
	gl_Position = pc.mvp * vec4(in_position, 1.0);
	frag_uv = in_uv;
	frag_texture_layer = int(in_texture_layer + 0.5);
	frag_light = in_light;
}
