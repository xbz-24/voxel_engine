		const glm::vec3 shadow_center = camera_position + camera_forward * (shadow_distance * 0.32f);
		const glm::vec3 light_position = shadow_center + sun_direction * (shadow_distance * 1.75f);
		const glm::vec3 world_up = std::abs(glm::dot(sun_direction, glm::vec3{ 0.0f, 1.0f, 0.0f })) > 0.96f
			? glm::vec3{ 0.0f, 0.0f, 1.0f }
			: glm::vec3{ 0.0f, 1.0f, 0.0f };
		const glm::mat4 light_view = glm::lookAtRH(light_position, shadow_center, world_up);
		glm::mat4 light_projection = glm::orthoRH_ZO(
			-shadow_distance,
			shadow_distance,
			-shadow_distance,
			shadow_distance,
			0.1f,
			shadow_distance * 4.0f);
		light_projection[1][1] *= -1.0f;
		const glm::mat4 unsnapped_light_matrix = light_projection * light_view;
		const glm::vec4 world_origin_clip = unsnapped_light_matrix * glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
		const float half_resolution = ve::core::ToFloat(VulkanDirectionalShadowMapResolution) * 0.5f;
		const glm::vec2 origin_texels = glm::vec2{ world_origin_clip } * half_resolution;
		const glm::vec2 rounded_origin_texels = glm::round(origin_texels);
		const glm::vec2 clip_offset = (rounded_origin_texels - origin_texels) / half_resolution;
		light_projection[3][0] += clip_offset.x;
		light_projection[3][1] += clip_offset.y;
		return light_projection * light_view;
	}

	[[nodiscard]] inline VulkanVoxelFrameUniforms PackVulkanVoxelFrameUniforms(
		const VoxelRenderStyle& style,
		glm::vec3 camera_position,
		glm::vec3 camera_forward,
		float elapsed_seconds,
		VkExtent2D viewport_extent) noexcept
	{
		const auto finite_non_negative = [](float value, float fallback) noexcept
		{
			return std::isfinite(value) && value >= 0.0f ? value : fallback;
		};
		const auto finite_non_negative_color = [&finite_non_negative](glm::vec3 color, glm::vec3 fallback) noexcept
		{
			return glm::vec3{
				finite_non_negative(color.x, fallback.x),
				finite_non_negative(color.y, fallback.y),
				finite_non_negative(color.z, fallback.z)
			};
		};

		const glm::vec3 sun_direction = NormalizeVulkanShaderDirection(
			style.sun_direction,
			{ -0.42f, 0.78f, -0.46f });

		const auto finite_in_range = [](float value, float fallback, float maximum) noexcept
		{
			return std::isfinite(value) ? std::clamp(value, 0.0f, maximum) : fallback;
		};
		camera_position.x = std::isfinite(camera_position.x) ? camera_position.x : 0.0f;
		camera_position.y = std::isfinite(camera_position.y) ? camera_position.y : 0.0f;
		camera_position.z = std::isfinite(camera_position.z) ? camera_position.z : 0.0f;
		const float frame_time = finite_non_negative(elapsed_seconds, 0.0f);
		const float sun_intensity = finite_non_negative(style.sun_intensity, 1.0f);
		const float exposure = std::max(finite_non_negative(style.exposure, 1.0f), 0.001f);
		const float fog_start_distance = finite_non_negative(style.fog_start_distance, 165.0f);
		const float fog_end_distance = std::max(
			finite_non_negative(style.fog_end_distance, 455.0f),
			fog_start_distance + 0.001f);
		const glm::vec3 sun_color = finite_non_negative_color(
			style.sun_color,
			{ 1.16f, 1.04f, 0.84f });
		const glm::vec3 sky_horizon_color = finite_non_negative_color(
			style.sky_horizon_color,
			{ 0.72f, 0.70f, 0.64f });
		const glm::vec3 sky_zenith_color = finite_non_negative_color(
			style.sky_zenith_color,
			{ 0.52f, 0.68f, 0.88f });
		const float viewport_width = std::max(ve::core::ToFloat(viewport_extent.width), 1.0f);
		const float viewport_height = std::max(ve::core::ToFloat(viewport_extent.height), 1.0f);
		const float directional_shadow_distance = std::isfinite(style.directional_shadow_distance)
			? std::clamp(style.directional_shadow_distance, 32.0f, 512.0f)
			: 220.0f;
		return VulkanVoxelFrameUniforms{
			glm::vec4{ camera_position, frame_time },
			glm::vec4{ sun_direction, sun_intensity },
			glm::vec4{ sun_color, exposure },
			glm::vec4{ sky_horizon_color, fog_start_distance },
			glm::vec4{ sky_zenith_color, fog_end_distance },
			glm::vec4{ viewport_width, viewport_height, 1.0f / viewport_width, 1.0f / viewport_height },
			glm::vec4{
				finite_in_range(style.fog_strength, 0.42f, 1.0f),
				finite_in_range(style.cloud_coverage, 0.42f, 1.0f),
				finite_in_range(style.cloud_density, 0.62f, 1.0f),
