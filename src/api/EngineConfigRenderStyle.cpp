#include "voxel/Engine.h"

#include "EngineRuntimeBridge.h"

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace voxel
{
VoxelRenderStyle& VoxelRenderStyle::WithSunDirection(Vec3 direction) noexcept
	{
		sun_direction = direction;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithSunColor(Vec3 color) noexcept
	{
		sun_color = color;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithSunIntensity(float intensity) noexcept
	{
		sun_intensity = intensity;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithSkyColors(Vec3 horizon, Vec3 zenith) noexcept
	{
		sky_horizon_color = horizon;
		sky_zenith_color = zenith;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithExposure(float value) noexcept
	{
		exposure = value;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithFogRange(float start_distance, float end_distance) noexcept
	{
		fog_start_distance = start_distance;
		fog_end_distance = end_distance;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithFogStrength(float strength) noexcept
	{
		fog_strength = strength;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithClouds(float coverage, float density, float speed) noexcept
	{
		cloud_coverage = coverage;
		cloud_density = density;
		cloud_speed = speed;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithSurfaceDetail(float strength) noexcept
	{
		surface_detail_strength = strength;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithWaterReflections(float strength) noexcept
	{
		water_reflection_strength = strength;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithShadowStrength(float strength) noexcept
	{
		shadow_strength = strength;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithSpecularStrength(float strength) noexcept
	{
		specular_strength = strength;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithAmbientOcclusion(float strength) noexcept
	{
		ambient_occlusion_strength = strength;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithAerialPerspective(float strength) noexcept
	{
		aerial_perspective_strength = strength;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithSubsurfaceScattering(float strength) noexcept
	{
		subsurface_scattering_strength = strength;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithCloudLightAbsorption(float strength) noexcept
	{
		cloud_light_absorption = strength;
		return *this;
	}

	VoxelRenderStyle& VoxelRenderStyle::WithDirectionalShadows(
		float strength,
		float softness,
		float distance) noexcept
	{
		directional_shadow_strength = strength;
		directional_shadow_softness = softness;
		directional_shadow_distance = distance;
		return *this;
	}

	EngineConfig& EngineConfig::WithVoxelRenderStyle(VoxelRenderStyle value) noexcept
	{
		voxel_render_style = value;
		return *this;
	}
}
