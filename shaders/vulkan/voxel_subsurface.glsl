#ifndef VE_VOXEL_SUBSURFACE_GLSL
#define VE_VOXEL_SUBSURFACE_GLSL

#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_surface_masks.glsl"

vec3 evaluate_voxel_subsurface(
	vec3 albedo,
	vec3 normal,
	vec3 view_direction,
	VoxelSurfaceMasks masks,
	VoxelEnvironment environment)
{
	float foliage = masks.greenery * (1.0 - masks.water) * (1.0 - masks.light_source);
	float snow = masks.snow * (1.0 - masks.water);
	float transmission_material = saturate(foliage + snow * 0.42);
	if (transmission_material <= 0.0001 || environment.subsurface_scattering_strength <= 0.0001)
	{
		return vec3(0.0);
	}

	float wrapped_backlight = saturate((dot(-normal, environment.sun_direction) + 0.38) / 1.38);
	float view_alignment = pow(saturate(dot(-view_direction, environment.sun_direction) * 0.5 + 0.5), 2.0);
	float transmission = wrapped_backlight * mix(0.58, 1.0, view_alignment);
	vec3 foliage_scatter = albedo * vec3(0.48, 1.14, 0.42);
	vec3 snow_scatter = mix(albedo, vec3(1.0, 0.83, 0.65), 0.32);
	vec3 scatter_color = mix(foliage_scatter, snow_scatter, saturate(snow));
	return scatter_color * environment.sun_color * environment.sun_intensity *
		transmission * transmission_material * environment.subsurface_scattering_strength * 0.13;
}

#endif
