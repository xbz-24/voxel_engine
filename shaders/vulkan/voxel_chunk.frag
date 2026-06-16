#version 450

layout(binding = 0) uniform sampler2DArray block_textures;

layout(location = 0) in vec2 frag_uv;
layout(location = 1) flat in int frag_texture_layer;
layout(location = 2) in float frag_light;

layout(location = 0) out vec4 out_color;

void main()
{
	vec4 texel = texture(block_textures, vec3(frag_uv, frag_texture_layer));
	if (texel.a < 0.05)
	{
		discard;
	}
	out_color = vec4(texel.rgb * frag_light, texel.a);
}
