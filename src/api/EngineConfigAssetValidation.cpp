#include "EngineConfigAssetSourceValidation.h"
#include "EngineConfigValidationInternal.h"

#include <concepts>
#include <ranges>
#include <set>
#include <string_view>
#include <type_traits>

namespace voxel::detail::config_validation
{
	namespace
	{
		template <typename AssetT>
		concept PublicAssetRecord = requires(const AssetT& asset) {
			{ asset.name } -> std::convertible_to<std::string_view>;
			{ asset.path } -> std::convertible_to<std::string_view>;
			{ asset.source } -> std::same_as<const AssetSource&>;
		};

		void ValidateAssetNameIsPresentAndUnique(std::string_view asset_name,
			std::string_view asset_kind,
			std::set<std::string>& asset_names_seen,
			std::vector<std::string>& issues)
		{
			if (asset_name.empty())
			{
				issues.push_back(std::string{ asset_kind } + " asset name must not be empty");
			}
			else if (!asset_names_seen.insert(std::string{ asset_name }).second)
			{
				issues.push_back(std::string{ asset_kind } + " asset name is duplicated: " + std::string{ asset_name });
			}
		}

		template <std::ranges::input_range AssetRange>
			requires PublicAssetRecord<std::remove_cvref_t<std::ranges::range_reference_t<AssetRange>>>
		void ValidateAssetRecords(const AssetRange& assets,
			std::string_view asset_kind,
			bool require_existing_files,
			std::vector<std::string>& issues)
		{
			std::set<std::string> asset_names_seen;
			for (const auto& asset_record : assets)
			{
				ValidateAssetNameIsPresentAndUnique(asset_record.name, asset_kind, asset_names_seen, issues);
				ValidateAssetSource(asset_record.source, asset_record.path, asset_kind, require_existing_files, issues);
			}
		}

	}

	void ValidateAssets(const std::vector<TextureAsset>& assets,
		std::string_view asset_kind,
		bool require_existing_files,
		std::vector<std::string>& issues)
	{
		ValidateAssetRecords(assets, asset_kind, require_existing_files, issues);
	}

	void ValidateAssets(const std::vector<ModelAsset>& assets,
		std::string_view asset_kind,
		bool require_existing_files,
		std::vector<std::string>& issues)
	{
		ValidateAssetRecords(assets, asset_kind, require_existing_files, issues);
	}

	void ValidateAssets(const std::vector<SoundAsset>& assets,
		std::string_view asset_kind,
		bool require_existing_files,
		std::vector<std::string>& issues)
	{
		ValidateAssetRecords(assets, asset_kind, require_existing_files, issues);
	}

}
