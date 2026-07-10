#ifndef VE_VOXEL_SPECULAR_GLSL
#define VE_VOXEL_SPECULAR_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_surface_masks.glsl"

float evaluate_voxel_specular(
	vec3 normal,
	float roughness,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	vec3 stylized_view_direction = normalize(vec3(0.18, 0.42, 0.54));
	vec3 half_direction = normalize(environment.sun_direction + stylized_view_direction);
	float roughness_squared = max(roughness * roughness, 0.02);
	float specular_power = mix(112.0, 5.0, roughness_squared);
	float normal_alignment = max(dot(normal, half_direction), 0.0);
	float lobe = pow(normal_alignment, specular_power);
	float material_response = 0.025 + masks.water * 0.34 + masks.snow * 0.055 + masks.light_source * 0.10;
	return lobe * material_response * environment.sun_intensity;
}

#endif
