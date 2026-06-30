namespace voxel
{
	namespace
	{
		class EngineConfigValidator final : public detail::IEngineConfigValidator
		{
		public:
			[[nodiscard]] std::vector<std::string> Validate(const EngineConfig& config) const override
			{
				std::vector<std::string> issues;
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
				if (config.world.size_chunks <= 0)
				{
					issues.push_back("world.size_chunks must be greater than zero");
				}
				if (config.render_distance_chunks < 0)
				{
					issues.push_back("render_distance_chunks must be zero or greater");
				}
				if (config.graphics_api == GraphicsApi::DirectX12)
				{
					issues.push_back("DirectX12 backend is declared but not implemented");
				}
				std::vector<std::string> asset_issues = config.assets.Validate();
				issues.insert(issues.end(), asset_issues.begin(), asset_issues.end());
				std::vector<std::string> material_issues = config.materials.Validate();
				issues.insert(issues.end(), material_issues.begin(), material_issues.end());
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
				return issues;
			}
		};
	}
