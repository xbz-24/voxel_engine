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
			{
				issues.push_back("voxel_render_style.sky_horizon_color must contain finite non-negative values");
			}
			if (!IsFiniteNonNegativeVector(style.sky_zenith_color))
			{
				issues.push_back("voxel_render_style.sky_zenith_color must contain finite non-negative values");
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
			if (!std::isfinite(style.fog_end_distance) ||
				style.fog_end_distance <= style.fog_start_distance)
			{
				issues.push_back("voxel_render_style.fog_end_distance must be finite and greater than fog_start_distance");
			}
		}

		void ValidateConfiguredPublicData(const EngineConfig& config, std::vector<std::string>& issues)
		{
			AppendValidationIssues(issues, config.assets.Validate());
			AppendValidationIssues(issues, config.materials.Validate());
			AppendValidationIssues(issues, config.scene_graph.Validate());
			ValidateMaterialTextureReferences(config.materials, config.assets, issues);
			ValidateSceneGraphAssetReferences(config.scene_graph, config.assets, config.materials, issues);
		}

		void ValidateRuntimeBindingSupportForConfiguredFeatures(const EngineConfig& config,
			std::vector<std::string>& issues)
		{
			if (!config.assets.textures.empty() || !config.assets.models.empty() || !config.assets.sounds.empty())
			{
				issues.push_back("AssetCatalog runtime loading is not implemented");
			}
			if (!config.materials.materials.empty())
			{
				issues.push_back("MaterialLibrary runtime binding is not implemented");
			}
			if (!config.scene_graph.entities.empty() || !config.scene_graph.lights.empty())
			{
				issues.push_back("SceneGraph runtime rendering is not implemented");
			}
		}

		class EngineConfigValidator final : public detail::IEngineConfigValidator
		{
		public:
			[[nodiscard]] std::vector<std::string> Validate(const EngineConfig& config) const override
			{
				std::vector<std::string> issues;
				ValidateWindowConfiguration(config, issues);
				ValidateWorldConfiguration(config, issues);
				ValidateGraphicsBackendSupport(config, issues);
				ValidateVoxelRenderStyle(config.voxel_render_style, issues);
				ValidateConfiguredPublicData(config, issues);
				ValidateRuntimeBindingSupportForConfiguredFeatures(config, issues);
				return issues;
			}
		};
	}
