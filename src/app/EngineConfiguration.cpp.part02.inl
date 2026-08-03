			if (!IsFiniteNonNegative(style.sun_color) ||
				!IsFiniteNonNegative(style.sky_horizon_color) ||
				!IsFiniteNonNegative(style.sky_zenith_color))
			{
				issues.push_back("voxel_render_style colors must contain finite non-negative values");
			}
			if (!std::isfinite(style.sun_intensity) || style.sun_intensity < 0.0f)
			{
				issues.push_back("voxel_render_style.sun_intensity must be finite and non-negative");
			}
			if (!std::isfinite(style.exposure) || style.exposure <= 0.0f)
			{
				issues.push_back("voxel_render_style.exposure must be finite and greater than zero");
			}
			if (!std::isfinite(style.fog_start_distance) || style.fog_start_distance < 0.0f)
			{
				issues.push_back("voxel_render_style.fog_start_distance must be finite and non-negative");
			}
			if (!std::isfinite(style.fog_end_distance) || style.fog_end_distance <= style.fog_start_distance)
			{
				issues.push_back("voxel_render_style.fog_end_distance must be finite and greater than fog_start_distance");
			}
			if (!IsFiniteInRange(style.fog_strength, 0.0f, 1.0f) ||
				!IsFiniteInRange(style.cloud_coverage, 0.0f, 1.0f) ||
				!IsFiniteInRange(style.cloud_density, 0.0f, 1.0f))
			{
				issues.push_back("voxel_render_style atmosphere strengths must be finite and between 0 and 1");
			}
			if (!std::isfinite(style.cloud_speed) || style.cloud_speed < 0.0f)
			{
				issues.push_back("voxel_render_style.cloud_speed must be finite and non-negative");
			}
			if (!IsFiniteInRange(style.surface_detail_strength, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.water_reflection_strength, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.shadow_strength, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.specular_strength, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.ambient_occlusion_strength, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.aerial_perspective_strength, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.subsurface_scattering_strength, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.cloud_light_absorption, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.directional_shadow_strength, 0.0f, 2.0f) ||
				!IsFiniteInRange(style.directional_shadow_softness, 0.0f, 2.0f))
			{
				issues.push_back("voxel_render_style surface strengths must be finite and between 0 and 2");
			}
			if (!IsFiniteInRange(style.directional_shadow_distance, 32.0f, 512.0f))
			{
				issues.push_back("voxel_render_style.directional_shadow_distance must be finite and between 32 and 512");
			}
		}
	}

	std::vector<std::string> ValidateEngineCreateInfo(const EngineCreateInfo& create_info)
	{
		std::vector<std::string> issues;
		if (create_info.window.width <= 0)
		{
			issues.push_back("window.width must be greater than zero");
		}
		if (create_info.window.height <= 0)
		{
			issues.push_back("window.height must be greater than zero");
		}
		if (create_info.window.display_index < 0)
		{
			issues.push_back("window.display_index must be zero or greater");
		}
		if (create_info.window.refresh_rate_hertz < 0)
		{
			issues.push_back("window.refresh_rate_hertz must be zero or greater");
		}
		if (!IsKnownGraphicsApi(create_info.render_backend.preferred_api))
		{
			issues.push_back("render_backend.preferred_api is not a known graphics api");
		}
		if (!IsKnownRenderBackendSelectionPolicy(create_info.render_backend.selection_policy))
		{
			issues.push_back("render_backend.selection_policy is not a known selection policy");
		}
		ValidateVoxelRenderStyle(create_info.voxel_render_style, issues);
		if (create_info.world_size_chunks <= 0)
		{
			issues.push_back("world_size_chunks must be greater than zero");
		}
