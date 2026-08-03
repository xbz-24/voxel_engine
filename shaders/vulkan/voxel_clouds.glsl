#ifndef VE_VOXEL_CLOUDS_GLSL
#define VE_VOXEL_CLOUDS_GLSL

#include "voxel_cloud_lighting.glsl"
#include "voxel_environment.glsl"
#include "voxel_math.glsl"
#include "voxel_noise.glsl"

vec4 evaluate_voxel_clouds(vec3 view_direction, VoxelEnvironment environment)
{
	float cloud_layer_height = 196.0;
	float height_to_cloud_layer = cloud_layer_height - environment.camera_position.y;
	if (view_direction.y <= 0.015 || height_to_cloud_layer <= 0.0 ||
		environment.cloud_coverage <= 0.001 || environment.cloud_density <= 0.001)
	{
		return vec4(0.0);
	}
	float layer_distance = height_to_cloud_layer / view_direction.y;
	vec2 world_sample = environment.camera_position.xz + view_direction.xz * layer_distance;
	vec2 wind = vec2(0.86, 0.51) * environment.elapsed_seconds * environment.cloud_speed;
	vec2 cloud_coordinate = world_sample * 0.0034 + wind;
	float base_shape = voxel_fbm(cloud_coordinate);
	float erosion = voxel_fbm(cloud_coordinate * 3.17 - wind * 0.38 + vec2(17.0, 5.0));
	float coverage_threshold = mix(0.78, 0.30, environment.cloud_coverage);
	float cloud_shape = smoothstep(coverage_threshold - 0.08, coverage_threshold + 0.10, base_shape);
	cloud_shape *= smoothstep(0.22, 0.72, erosion + cloud_shape * 0.28);
	float horizon_fade = smoothstep(0.015, 0.16, view_direction.y);
	float density = saturate(cloud_shape * environment.cloud_density * 1.45) * horizon_fade;
	return evaluate_voxel_cloud_lighting(
		view_direction,
		cloud_coordinate,
		density,
		erosion,
		environment);
}

#endif
