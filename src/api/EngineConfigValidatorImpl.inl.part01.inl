namespace voxel
{
	namespace
	{
		void AppendValidationIssues(std::vector<std::string>& destination_issues,
			std::vector<std::string> source_issues)
		{
			destination_issues.insert(destination_issues.end(), source_issues.begin(), source_issues.end());
		}

		void ValidateWindowConfiguration(const EngineConfig& config, std::vector<std::string>& issues)
		{
			if (config.window.width <= 0)
			{
				issues.push_back("window.width must be greater than zero");
			}
			if (config.window.height <= 0)
			{
				issues.push_back("window.height must be greater than zero");
			}
			if (config.window.monitor_index < 0)
			{
				issues.push_back("window.monitor_index must be zero or greater");
			}
			if (config.window.refresh_rate_hertz < 0)
			{
				issues.push_back("window.refresh_rate_hertz must be zero or greater");
			}
		}

		void ValidateWorldConfiguration(const EngineConfig& config, std::vector<std::string>& issues)
		{
			if (config.world.size_chunks <= 0)
			{
				issues.push_back("world.size_chunks must be greater than zero");
			}
			if (config.render_distance_chunks < 0)
			{
				issues.push_back("render_distance_chunks must be zero or greater");
			}
		}

		void ValidateGraphicsBackendSupport(const EngineConfig& config, std::vector<std::string>& issues)
		{
			if (config.graphics_api == GraphicsApi::DirectX12)
			{
				issues.push_back("DirectX12 backend is declared but not implemented");
			}
		}

		[[nodiscard]] bool IsFiniteVector(Vec3 value) noexcept
		{
			return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
		}

		[[nodiscard]] bool IsFiniteNonNegativeVector(Vec3 value) noexcept
		{
			return IsFiniteVector(value) && value.x >= 0.0f && value.y >= 0.0f && value.z >= 0.0f;
		}

		void ValidateVoxelRenderStyle(const VoxelRenderStyle& style, std::vector<std::string>& issues)
		{
			const auto validate_normalized = [&issues](float value, const char* label)
			{
				if (!std::isfinite(value) || value < 0.0f || value > 1.0f)
				{
					issues.push_back(std::string{ label } + " must be finite and between 0 and 1");
				}
			};
			const auto validate_response_strength = [&issues](float value, const char* label)
			{
				if (!std::isfinite(value) || value < 0.0f || value > 2.0f)
				{
					issues.push_back(std::string{ label } + " must be finite and between 0 and 2");
				}
			};
			const float largest_sun_direction_component = std::max(
				std::abs(style.sun_direction.x),
				std::max(std::abs(style.sun_direction.y), std::abs(style.sun_direction.z)));
			if (!IsFiniteVector(style.sun_direction) || largest_sun_direction_component <= 0.000001f)
			{
				issues.push_back("voxel_render_style.sun_direction must be finite and non-zero");
			}
			if (!IsFiniteNonNegativeVector(style.sun_color))
			{
				issues.push_back("voxel_render_style.sun_color must contain finite non-negative values");
			}
			if (!IsFiniteNonNegativeVector(style.sky_horizon_color))
