#include "EngineRuntimeBridge.h"

#include <filesystem>
#include <set>
#include <string_view>
#include <utility>

namespace voxel
{
	namespace
	{
		template <typename AssetT>
		void ValidateAssets(const std::vector<AssetT>& assets,
			std::string_view kind,
			bool require_existing_files,
			std::vector<std::string>& issues)
		{
			std::set<std::string> names;
			for (const AssetT& asset : assets)
			{
				if (asset.name.empty())
				{
					issues.push_back(std::string{ kind } + " asset name must not be empty");
				}
				else if (!names.insert(asset.name).second)
				{
					issues.push_back(std::string{ kind } + " asset name is duplicated: " + asset.name);
				}
				if (asset.path.empty())
				{
					issues.push_back(std::string{ kind } + " asset path must not be empty");
				}
				else if (require_existing_files && !std::filesystem::exists(asset.path))
				{
					issues.push_back(std::string{ kind } + " asset path does not exist: " + asset.path);
				}
			}
		}

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

	std::vector<std::string> AssetCatalog::Validate(bool require_existing_files) const
	{
		std::vector<std::string> issues;
		ValidateAssets(textures, "texture", require_existing_files, issues);
		ValidateAssets(models, "model", require_existing_files, issues);
		ValidateAssets(sounds, "sound", require_existing_files, issues);
		return issues;
	}

	std::vector<std::string> EngineConfig::Validate() const
	{
		return detail::DefaultEngineConfigValidator().Validate(*this);
	}

	bool EngineConfig::IsValid() const
	{
		return Validate().empty();
	}
}

namespace voxel::detail
{
	const IEngineConfigValidator& DefaultEngineConfigValidator() noexcept
	{
		static const EngineConfigValidator validator{};
		return validator;
	}
}
