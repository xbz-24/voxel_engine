#include "voxel/Engine.h"

namespace voxel
{
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
}
