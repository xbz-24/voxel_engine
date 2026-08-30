#ifndef VE_VOXEL_BRDF_GLSL
#define VE_VOXEL_BRDF_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_surface_masks.glsl"

const float VoxelPi = 3.14159265359;

vec3 voxel_fresnel_schlick(float cosine, vec3 reflectance_at_normal_incidence)
{
	float grazing = pow(1.0 - saturate(cosine), 5.0);
	return reflectance_at_normal_incidence + (vec3(1.0) - reflectance_at_normal_incidence) * grazing;
}

float voxel_ggx_distribution(float normal_half_alignment, float roughness)
{
	float alpha = max(roughness * roughness, 0.0025);
	float alpha_squared = alpha * alpha;
	float denominator = normal_half_alignment * normal_half_alignment * (alpha_squared - 1.0) + 1.0;
	return alpha_squared / max(VoxelPi * denominator * denominator, 0.0001);
}

float voxel_smith_geometry(float normal_alignment, float roughness)
{
	float radius = roughness + 1.0;
	float k = (radius * radius) * 0.125;
	return normal_alignment / max(normal_alignment * (1.0 - k) + k, 0.0001);
}

vec3 evaluate_voxel_ggx_specular(
	vec3 albedo,
	vec3 normal,
	vec3 view_direction,
	float roughness,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	vec3 light_direction = environment.sun_direction;
	float normal_view = max(dot(normal, view_direction), 0.0);
	float normal_light = max(dot(normal, light_direction), 0.0);
	if (normal_view <= 0.0001 || normal_light <= 0.0001)
	{
		return vec3(0.0);
	}
	vec3 half_vector = view_direction + light_direction;
	float half_length_squared = max(dot(half_vector, half_vector), 0.000001);
	vec3 half_direction = half_vector * inversesqrt(half_length_squared);
	float normal_half = max(dot(normal, half_direction), 0.0);
	float view_half = max(dot(view_direction, half_direction), 0.0);
	vec3 dielectric_f0 = mix(vec3(0.035), vec3(0.020), masks.water);
	vec3 f0 = mix(dielectric_f0, albedo, masks.light_source * 0.10);
	vec3 fresnel = voxel_fresnel_schlick(view_half, f0);
	float distribution = voxel_ggx_distribution(normal_half, roughness);
	float geometry = voxel_smith_geometry(normal_view, roughness) *
		voxel_smith_geometry(normal_light, roughness);
	vec3 specular = (distribution * geometry * fresnel) /
		max(4.0 * normal_view * normal_light, 0.001);
	return specular * environment.sun_color * environment.sun_intensity *
		normal_light * environment.specular_strength;
}

#endif
