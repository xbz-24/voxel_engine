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
