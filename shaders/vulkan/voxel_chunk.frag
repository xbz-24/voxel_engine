#version 450

#extension GL_GOOGLE_include_directive : require

#include "voxel_math.glsl"
#include "voxel_environment.glsl"
#include "voxel_surface_masks.glsl"
#include "voxel_surface_detail.glsl"
#include "voxel_shadowing.glsl"
#include "voxel_materials.glsl"
#include "voxel_lighting.glsl"
#include "voxel_atmosphere.glsl"
#include "voxel_grade.glsl"

layout(location = 0) in vec4 frag_color;
layout(location = 1) in float frag_light;
layout(location = 2) in vec3 frag_normal;
layout(location = 3) in float frag_view_depth;
layout(location = 4) in vec3 frag_world_position;

layout(location = 0) out vec4 out_color;

void main()
{
	if (frag_color.a < 0.05)
	{
		discard;
	}
	vec3 normal = normalize(frag_normal);
	float vertex_light = clamp(frag_light, 0.0, 1.70);
	float height_blend = smoothstep(38.0, 118.0, frag_world_position.y);
	VoxelSurfaceMasks surface_masks = build_voxel_surface_masks(frag_color, normal, frag_world_position, height_blend);
	VoxelEnvironment environment = current_voxel_environment();

	vec3 albedo = apply_voxel_material_tint(frag_color.rgb, frag_world_position, normal, height_blend, surface_masks);
	vec3 lit = evaluate_voxel_lighting(albedo, normal, vertex_light, height_blend, frag_world_position, surface_masks, environment);
	float ordered_grain = (hash13(floor(frag_world_position * 0.73)) - 0.5) * 0.020;
	float screen_grain = screen_dither(gl_FragCoord.xy) * 0.006;
	vec3 final_color = apply_voxel_atmosphere(
		lit + vec3(ordered_grain + screen_grain),
		frag_view_depth,
		frag_world_position,
		height_blend,
		environment);
	final_color = apply_voxel_color_grade(final_color, frag_world_position, surface_masks);
	out_color = vec4(saturate(final_color), frag_color.a);
}
