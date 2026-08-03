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
			validate_normalized(style.fog_strength, "voxel_render_style.fog_strength");
			validate_normalized(style.cloud_coverage, "voxel_render_style.cloud_coverage");
			validate_normalized(style.cloud_density, "voxel_render_style.cloud_density");
			if (!std::isfinite(style.cloud_speed) || style.cloud_speed < 0.0f)
			{
				issues.push_back("voxel_render_style.cloud_speed must be finite and non-negative");
			}
			validate_response_strength(style.surface_detail_strength, "voxel_render_style.surface_detail_strength");
			validate_response_strength(style.water_reflection_strength, "voxel_render_style.water_reflection_strength");
			validate_response_strength(style.shadow_strength, "voxel_render_style.shadow_strength");
			validate_response_strength(style.specular_strength, "voxel_render_style.specular_strength");
			validate_response_strength(style.ambient_occlusion_strength, "voxel_render_style.ambient_occlusion_strength");
			validate_response_strength(style.aerial_perspective_strength, "voxel_render_style.aerial_perspective_strength");
			validate_response_strength(style.subsurface_scattering_strength, "voxel_render_style.subsurface_scattering_strength");
			validate_response_strength(style.cloud_light_absorption, "voxel_render_style.cloud_light_absorption");
			validate_response_strength(style.directional_shadow_strength, "voxel_render_style.directional_shadow_strength");
			validate_response_strength(style.directional_shadow_softness, "voxel_render_style.directional_shadow_softness");
			if (!std::isfinite(style.directional_shadow_distance) ||
				style.directional_shadow_distance < 32.0f || style.directional_shadow_distance > 512.0f)
			{
				issues.push_back("voxel_render_style.directional_shadow_distance must be finite and between 32 and 512");
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
