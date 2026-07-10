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

	EngineConfig& EngineConfig::WithVoxelRenderStyle(VoxelRenderStyle value) noexcept
	{
		voxel_render_style = value;
		return *this;
	}
