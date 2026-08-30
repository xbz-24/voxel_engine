#version 450

layout(location = 0) out vec2 frag_ndc;

void main()
{
	const vec2 full_screen_triangle[3] = vec2[](
		vec2(-1.0, -1.0),
		vec2(3.0, -1.0),
		vec2(-1.0, 3.0));
	frag_ndc = full_screen_triangle[gl_VertexIndex];
	gl_Position = vec4(frag_ndc, 1.0, 1.0);
}
