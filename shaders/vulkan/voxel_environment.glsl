#ifndef VE_VOXEL_ENVIRONMENT_GLSL
#define VE_VOXEL_ENVIRONMENT_GLSL

layout(set = 0, binding = 0, std140) uniform VoxelFrameUniforms
{
	vec4 camera_position_and_time;
	vec4 sun_direction_and_intensity;
	vec4 sun_color_and_exposure;
	vec4 sky_horizon_color_and_fog_start;
	vec4 sky_zenith_color_and_fog_end;
	vec4 viewport_size_and_inverse;
	vec4 atmosphere_parameters;
	vec4 surface_parameters;
	vec4 advanced_lighting_parameters;
	vec4 directional_shadow_parameters;
	mat4 light_view_projection;
} voxel_frame;

struct VoxelEnvironment
{
	vec3 sun_direction;
	vec3 sun_color;
	vec3 sky_horizon_color;
	vec3 sky_zenith_color;
	vec3 camera_position;
	vec2 viewport_size;
	vec2 inverse_viewport_size;
	float sun_intensity;
	float exposure;
	float fog_start_distance;
	float fog_end_distance;
	float elapsed_seconds;
	float fog_strength;
	float cloud_coverage;
	float cloud_density;
	float cloud_speed;
	float surface_detail_strength;
	float water_reflection_strength;
	float shadow_strength;
	float specular_strength;
	float ambient_occlusion_strength;
	float aerial_perspective_strength;
	float subsurface_scattering_strength;
	float cloud_light_absorption;
	float directional_shadow_strength;
	float directional_shadow_softness;
	float directional_shadow_distance;
	float directional_shadow_texel_size;
};

VoxelEnvironment current_voxel_environment()
{
	VoxelEnvironment environment;
	float direction_length_squared = dot(
		voxel_frame.sun_direction_and_intensity.xyz,
		voxel_frame.sun_direction_and_intensity.xyz);
	environment.sun_direction = direction_length_squared > 0.000001
		? voxel_frame.sun_direction_and_intensity.xyz * inversesqrt(direction_length_squared)
		: normalize(vec3(-0.42, 0.78, -0.46));
	environment.sun_intensity = max(voxel_frame.sun_direction_and_intensity.w, 0.0);
	environment.sun_color = max(voxel_frame.sun_color_and_exposure.xyz, vec3(0.0));
	environment.exposure = max(voxel_frame.sun_color_and_exposure.w, 0.001);
	environment.sky_horizon_color = max(voxel_frame.sky_horizon_color_and_fog_start.xyz, vec3(0.0));
	environment.sky_zenith_color = max(voxel_frame.sky_zenith_color_and_fog_end.xyz, vec3(0.0));
	environment.camera_position = voxel_frame.camera_position_and_time.xyz;
	environment.viewport_size = max(voxel_frame.viewport_size_and_inverse.xy, vec2(1.0));
	environment.inverse_viewport_size = max(voxel_frame.viewport_size_and_inverse.zw, vec2(0.0));
	environment.elapsed_seconds = max(voxel_frame.camera_position_and_time.w, 0.0);
	environment.fog_start_distance = max(voxel_frame.sky_horizon_color_and_fog_start.w, 0.0);
	environment.fog_end_distance = max(
		voxel_frame.sky_zenith_color_and_fog_end.w,
		environment.fog_start_distance + 0.001);
	environment.fog_strength = clamp(voxel_frame.atmosphere_parameters.x, 0.0, 1.0);
	environment.cloud_coverage = clamp(voxel_frame.atmosphere_parameters.y, 0.0, 1.0);
	environment.cloud_density = clamp(voxel_frame.atmosphere_parameters.z, 0.0, 1.0);
	environment.cloud_speed = max(voxel_frame.atmosphere_parameters.w, 0.0);
	environment.surface_detail_strength = clamp(voxel_frame.surface_parameters.x, 0.0, 2.0);
	environment.water_reflection_strength = clamp(voxel_frame.surface_parameters.y, 0.0, 2.0);
	environment.shadow_strength = clamp(voxel_frame.surface_parameters.z, 0.0, 2.0);
	environment.specular_strength = clamp(voxel_frame.surface_parameters.w, 0.0, 2.0);
	environment.ambient_occlusion_strength = clamp(voxel_frame.advanced_lighting_parameters.x, 0.0, 2.0);
	environment.aerial_perspective_strength = clamp(voxel_frame.advanced_lighting_parameters.y, 0.0, 2.0);
	environment.subsurface_scattering_strength = clamp(voxel_frame.advanced_lighting_parameters.z, 0.0, 2.0);
	environment.cloud_light_absorption = clamp(voxel_frame.advanced_lighting_parameters.w, 0.0, 2.0);
	environment.directional_shadow_strength = clamp(voxel_frame.directional_shadow_parameters.x, 0.0, 2.0);
	environment.directional_shadow_softness = clamp(voxel_frame.directional_shadow_parameters.y, 0.0, 2.0);
	environment.directional_shadow_distance = max(voxel_frame.directional_shadow_parameters.z, 32.0);
	environment.directional_shadow_texel_size = max(voxel_frame.directional_shadow_parameters.w, 0.000001);
	return environment;
}

#endif
