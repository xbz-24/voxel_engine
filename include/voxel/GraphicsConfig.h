#pragma once

#include "voxel/Camera.h"

namespace voxel
{
	/** Requested graphics backend for EngineConfig startup. */
	enum class GraphicsApi
	{
		Vulkan,
		OpenGLCompatibility,
		DirectX12
	};

	/** Backend-neutral lighting, sky, tone-mapping, and fog settings for voxel rendering. */
	struct VoxelRenderStyle
	{
		Vec3 sun_direction{ -0.42f, 0.78f, -0.46f };
		Vec3 sun_color{ 1.16f, 1.04f, 0.84f };
		Vec3 sky_horizon_color{ 0.72f, 0.70f, 0.64f };
		Vec3 sky_zenith_color{ 0.52f, 0.68f, 0.88f };
		float sun_intensity = 1.0f;
		float exposure = 1.0f;
		float fog_start_distance = 165.0f;
		float fog_end_distance = 455.0f;
		float fog_strength = 0.42f;
		float cloud_coverage = 0.42f;
		float cloud_density = 0.62f;
		float cloud_speed = 0.018f;
		float surface_detail_strength = 1.0f;
		float water_reflection_strength = 1.0f;
		float shadow_strength = 1.0f;
		float specular_strength = 1.0f;
		float ambient_occlusion_strength = 1.0f;
		float aerial_perspective_strength = 1.0f;
		float subsurface_scattering_strength = 1.0f;
		float cloud_light_absorption = 1.0f;
		float directional_shadow_strength = 0.85f;
		float directional_shadow_softness = 1.0f;
		float directional_shadow_distance = 220.0f;

		VoxelRenderStyle& WithSunDirection(Vec3 direction) noexcept;
		VoxelRenderStyle& WithSunColor(Vec3 color) noexcept;
		VoxelRenderStyle& WithSunIntensity(float intensity) noexcept;
		VoxelRenderStyle& WithSkyColors(Vec3 horizon, Vec3 zenith) noexcept;
		VoxelRenderStyle& WithExposure(float value) noexcept;
		VoxelRenderStyle& WithFogRange(float start_distance, float end_distance) noexcept;
		VoxelRenderStyle& WithFogStrength(float strength) noexcept;
		VoxelRenderStyle& WithClouds(float coverage, float density, float speed) noexcept;
		VoxelRenderStyle& WithSurfaceDetail(float strength) noexcept;
		VoxelRenderStyle& WithWaterReflections(float strength) noexcept;
		VoxelRenderStyle& WithShadowStrength(float strength) noexcept;
		VoxelRenderStyle& WithSpecularStrength(float strength) noexcept;
		VoxelRenderStyle& WithAmbientOcclusion(float strength) noexcept;
		VoxelRenderStyle& WithAerialPerspective(float strength) noexcept;
		VoxelRenderStyle& WithSubsurfaceScattering(float strength) noexcept;
		VoxelRenderStyle& WithCloudLightAbsorption(float strength) noexcept;
		VoxelRenderStyle& WithDirectionalShadows(float strength, float softness, float distance) noexcept;
	};
}
