#version 450

layout(location = 0) in vec4 frag_color;
layout(location = 1) in float frag_light;

layout(location = 0) out vec4 out_color;

void main()
{
	if (frag_color.a < 0.05)
	{
		discard;
	}
	vec3 light_color = mix(vec3(0.68, 0.76, 0.88), vec3(1.08, 1.03, 0.94), clamp(frag_light, 0.0, 1.0));
	vec3 lit = frag_color.rgb * frag_light * light_color;
	lit = max(lit - vec3(0.030), vec3(0.0));
	lit = ((lit - vec3(0.5)) * 1.12) + vec3(0.5);
	lit = pow(clamp(lit, vec3(0.0), vec3(1.0)), vec3(0.86));
	out_color = vec4(clamp(lit, vec3(0.0), vec3(1.0)), frag_color.a);
}
