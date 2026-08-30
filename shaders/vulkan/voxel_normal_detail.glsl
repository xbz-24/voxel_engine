#ifndef VE_VOXEL_NORMAL_DETAIL_GLSL
#define VE_VOXEL_NORMAL_DETAIL_GLSL

#include "voxel_detail_filter.glsl"
#include "voxel_environment.glsl"
#include "voxel_noise.glsl"
#include "voxel_surface_masks.glsl"

void voxel_surface_basis(vec3 normal, out vec3 tangent, out vec3 bitangent)
{
	vec3 reference_axis = abs(normal.y) < 0.90 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	tangent = normalize(cross(reference_axis, normal));
	bitangent = normalize(cross(normal, tangent));
}

vec3 perturb_voxel_surface_normal(
	vec3 geometric_normal,
	vec3 world_position,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	if (environment.surface_detail_strength <= 0.0001)
	{
		return geometric_normal;
	}

	vec3 tangent;
	vec3 bitangent;
	voxel_surface_basis(geometric_normal, tangent, bitangent);
	vec2 surface_coordinate = vec2(dot(world_position, tangent), dot(world_position, bitangent));
	float detail_visibility = voxel_procedural_detail_visibility(
		world_position,
		surface_coordinate,
		environment);
	float solid_material = saturate(masks.stone * 0.70 + masks.dryness * 0.45 + masks.greenery * 0.24);
	float detail_strength = solid_material * environment.surface_detail_strength * detail_visibility * 0.16;
	vec3 detailed_normal = geometric_normal;
	if (detail_strength > 0.0001)
	{
		const float sample_step = 0.055;
		vec2 noise_coordinate = surface_coordinate * 0.72;
		float noise_center = voxel_detail_fbm(noise_coordinate);
		float noise_right = voxel_detail_fbm(noise_coordinate + vec2(sample_step, 0.0));
		float noise_up = voxel_detail_fbm(noise_coordinate + vec2(0.0, sample_step));
		vec2 gradient = vec2(noise_right - noise_center, noise_up - noise_center) / sample_step;
		detailed_normal = normalize(
			geometric_normal - tangent * gradient.x * detail_strength - bitangent * gradient.y * detail_strength);
	}

	float wave_time = environment.elapsed_seconds * 1.35;
	float first_wave = sin(dot(surface_coordinate, vec2(1.73, 1.12)) * 2.1 + wave_time);
	float second_wave = cos(dot(surface_coordinate, vec2(-1.08, 1.94)) * 2.7 - wave_time * 1.21);
	vec2 wave_gradient = vec2(first_wave, second_wave) * 0.075 *
		environment.surface_detail_strength * detail_visibility;
	vec3 water_normal = normalize(
		geometric_normal + tangent * wave_gradient.x + bitangent * wave_gradient.y);
	return normalize(mix(detailed_normal, water_normal, masks.water));
}

#endif
