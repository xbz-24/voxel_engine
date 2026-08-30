#ifndef VE_VOXEL_DIFFUSE_GLSL
#define VE_VOXEL_DIFFUSE_GLSL

#include "voxel_math.glsl"

float evaluate_voxel_rough_diffuse(
	vec3 normal,
	vec3 view_direction,
	vec3 light_direction,
	float roughness)
{
	float normal_light = max(dot(normal, light_direction), 0.0);
	float normal_view = max(dot(normal, view_direction), 0.0);
	if (normal_light <= 0.0001 || normal_view <= 0.0001)
	{
		return 0.0;
	}

	float roughness_squared = roughness * roughness;
	float oren_a = 1.0 - (0.50 * roughness_squared) / (roughness_squared + 0.33);
	float oren_b = (0.45 * roughness_squared) / (roughness_squared + 0.09);
	vec3 projected_view = view_direction - normal * normal_view;
	vec3 projected_light = light_direction - normal * normal_light;
	float projected_view_length_squared = dot(projected_view, projected_view);
	float projected_light_length_squared = dot(projected_light, projected_light);
	float azimuth_alignment = 0.0;
	if (projected_view_length_squared > 0.000001 && projected_light_length_squared > 0.000001)
	{
		azimuth_alignment = max(
			dot(projected_view, projected_light) *
			inversesqrt(projected_view_length_squared * projected_light_length_squared),
			0.0);
	}

	float view_sine = sqrt(max(1.0 - normal_view * normal_view, 0.0));
	float light_sine = sqrt(max(1.0 - normal_light * normal_light, 0.0));
	float sine_alpha = max(view_sine, light_sine);
	float tangent_beta = min(
		view_sine / max(normal_view, 0.08),
		light_sine / max(normal_light, 0.08));
	float rough_response = oren_a + oren_b * azimuth_alignment * sine_alpha * min(tangent_beta, 2.0);
	return normal_light * clamp(rough_response, 0.0, 1.35);
}

#endif
