#include "EngineRuntimeBridge.h"
#include "EngineConfigValidationInternal.h"

namespace voxel::detail::config_validation
{
	namespace
	{
		void AppendValidationIssues(std::vector<std::string>& destination_issues,
			std::vector<std::string> source_issues)
		{
			destination_issues.insert(destination_issues.end(), source_issues.begin(), source_issues.end());
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

		class EngineConfigValidator final : public IEngineConfigValidator
		{
		public:
			[[nodiscard]] std::vector<std::string> Validate(const EngineConfig& config) const override
			{
				std::vector<std::string> issues;
				ValidateWindowAndWorldConfiguration(config, issues);
				ValidateGraphicsBackendSupport(config, issues);
				ValidateVoxelRenderLightingAndFog(config.voxel_render_style, issues);
				ValidateVoxelRenderEffects(config.voxel_render_style, issues);
				ValidateConfiguredPublicData(config, issues);
				ValidateRuntimeBindingSupportForConfiguredFeatures(config, issues);
				return issues;
			}
		};
	}

	const IEngineConfigValidator& ValidatorInstance() noexcept
	{
		static const EngineConfigValidator validator{};
		return validator;
	}
}
