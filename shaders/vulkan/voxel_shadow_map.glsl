#ifndef VE_VOXEL_SHADOW_MAP_GLSL
#define VE_VOXEL_SHADOW_MAP_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"

layout(set = 0, binding = 1) uniform sampler2DShadow voxel_directional_shadow_map;

float evaluate_voxel_directional_shadow(
	vec3 world_position,
	vec3 normal,
	VoxelEnvironment environment)
{
	if (environment.directional_shadow_strength <= 0.0001)
	{
		return 1.0;
	}
	vec4 light_clip_position = voxel_frame.light_view_projection * vec4(world_position, 1.0);
	if (abs(light_clip_position.w) <= 0.000001)
	{
		return 1.0;
	}
	vec3 light_ndc = light_clip_position.xyz / light_clip_position.w;
	vec2 shadow_uv = light_ndc.xy * 0.5 + 0.5;
	if (light_ndc.z <= 0.0 || light_ndc.z >= 1.0 ||
		any(lessThan(shadow_uv, vec2(0.0))) || any(greaterThan(shadow_uv, vec2(1.0))))
	{
		return 1.0;
	}

	float normal_light = saturate(dot(normal, environment.sun_direction));
	float receiver_bias = max(
		environment.directional_shadow_texel_size * (1.10 + (1.0 - normal_light) * 2.8),
		0.00005);
	float filter_radius = mix(
		0.72,
		2.35,
		saturate(environment.directional_shadow_softness * 0.5));
	float visibility = 0.0;
	for (int offset_y = -1; offset_y <= 1; ++offset_y)
	{
		for (int offset_x = -1; offset_x <= 1; ++offset_x)
		{
			vec2 sample_offset = vec2(float(offset_x), float(offset_y)) *
				environment.directional_shadow_texel_size * filter_radius;
			visibility += texture(
				voxel_directional_shadow_map,
				vec3(shadow_uv + sample_offset, light_ndc.z - receiver_bias));
		}
	}
	visibility /= 9.0;
	float occlusion = (1.0 - visibility) * environment.directional_shadow_strength;
	return saturate(1.0 - occlusion);
}

#endif
