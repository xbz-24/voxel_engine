#ifndef VE_VOXEL_ENVIRONMENT_GLSL
#define VE_VOXEL_ENVIRONMENT_GLSL

layout(push_constant) uniform VoxelPushConstants
{
	mat4 transform;
	vec4 sun_direction_and_intensity;
	vec4 sun_color_and_exposure;
	vec4 sky_horizon_color_and_fog_start;
	vec4 sky_zenith_color_and_fog_end;
} voxel_push_constants;

struct VoxelEnvironment
{
	vec3 sun_direction;
	vec3 sun_color;
	vec3 sky_horizon_color;
	vec3 sky_zenith_color;
	float sun_intensity;
	float exposure;
	float fog_start_distance;
	float fog_end_distance;
};

VoxelEnvironment current_voxel_environment()
{
	VoxelEnvironment environment;
	float direction_length_squared = dot(
		voxel_push_constants.sun_direction_and_intensity.xyz,
		voxel_push_constants.sun_direction_and_intensity.xyz);
	environment.sun_direction = direction_length_squared > 0.000001
		? voxel_push_constants.sun_direction_and_intensity.xyz * inversesqrt(direction_length_squared)
		: normalize(vec3(-0.42, 0.78, -0.46));
	environment.sun_intensity = max(voxel_push_constants.sun_direction_and_intensity.w, 0.0);
	environment.sun_color = max(voxel_push_constants.sun_color_and_exposure.xyz, vec3(0.0));
	environment.exposure = max(voxel_push_constants.sun_color_and_exposure.w, 0.001);
	environment.sky_horizon_color = max(voxel_push_constants.sky_horizon_color_and_fog_start.xyz, vec3(0.0));
	environment.sky_zenith_color = max(voxel_push_constants.sky_zenith_color_and_fog_end.xyz, vec3(0.0));
	environment.fog_start_distance = max(voxel_push_constants.sky_horizon_color_and_fog_start.w, 0.0);
	environment.fog_end_distance = max(
		voxel_push_constants.sky_zenith_color_and_fog_end.w,
		environment.fog_start_distance + 0.001);
	return environment;
}

#endif
