#version 450

layout(location = 0) in vec4 frag_color;
layout(location = 1) in float frag_light;
layout(location = 2) in vec3 frag_normal;
layout(location = 3) in float frag_view_depth;

layout(location = 0) out vec4 out_color;

vec3 saturate(vec3 value)
{
	return clamp(value, vec3(0.0), vec3(1.0));
}

void main()
{
	if (frag_color.a < 0.05)
	{
		discard;
	}
	vec3 normal = normalize(frag_normal);
	vec3 sun_direction = normalize(vec3(-0.42, 0.78, -0.46));
	float direct_light = max(dot(normal, sun_direction), 0.0);
	float sky_bounce = clamp((normal.y * 0.5) + 0.5, 0.0, 1.0);
	float side_fill = 1.0 - abs(normal.y);
	float vertex_light = clamp(frag_light, 0.0, 1.70);

	vec3 cool_shadow = vec3(0.58, 0.69, 0.92);
	vec3 warm_sun = vec3(1.12, 1.02, 0.86);
	vec3 light_color = mix(cool_shadow, warm_sun, smoothstep(0.05, 0.95, direct_light));
	float ambient = mix(0.36, 0.62, sky_bounce);
	float diffuse = mix(0.18, 0.70, direct_light);
	float rim = 0.055 * side_fill * sky_bounce;

	vec3 lit = frag_color.rgb * vertex_light * (ambient + diffuse + rim) * light_color;
	lit = max(lit - vec3(0.018), vec3(0.0));
	float luminance = dot(lit, vec3(0.2126, 0.7152, 0.0722));
	lit = mix(vec3(luminance), lit, 1.08);
	lit = pow(saturate(lit), vec3(0.88));

	vec3 fog_color = vec3(0.62, 0.73, 0.84);
	float fog = smoothstep(170.0, 430.0, frag_view_depth);
	vec3 final_color = mix(saturate(lit), fog_color, fog * 0.42);
	out_color = vec4(saturate(final_color), frag_color.a);
}
