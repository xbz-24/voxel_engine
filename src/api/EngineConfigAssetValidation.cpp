#include "EngineRuntimeBridge.h"

#include <algorithm>
#include <concepts>
#include <cmath>
#include <filesystem>
#include <ranges>
#include <set>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace voxel
{
	namespace
	{
		template <typename AssetT>
		concept PublicAssetRecord = requires(const AssetT& asset) {
			{ asset.name } -> std::convertible_to<std::string_view>;
			{ asset.path } -> std::convertible_to<std::string_view>;
			{ asset.source } -> std::same_as<const AssetSource&>;
		};

		[[nodiscard]] std::string AssetLocation(const AssetSource& source, std::string_view legacy_path)
		{
			return source.location.empty() ? std::string{ legacy_path } : source.location;
		}

		void ValidateAssetHotReloadSource(const AssetSource& source,
			std::string_view asset_kind,
			std::vector<std::string>& issues)
		{
			if (source.hot_reload && source.storage != AssetStorage::FilePath)
			{
				issues.push_back(std::string{ asset_kind } + " asset hot reload requires a filesystem path");
			}
		}

		void ValidateEmbeddedAssetSource(const AssetSource& source,
			std::string_view asset_kind,
			std::vector<std::string>& issues)
		{
			if (source.embedded_data.empty())
			{
				issues.push_back(std::string{ asset_kind } + " embedded asset data must not be empty");
			}
		}

		void ValidatePackagedArchiveAssetSource(const AssetSource& source,
			const std::string& asset_location,
			std::string_view asset_kind,
			std::vector<std::string>& issues)
		{
			if (source.archive_path.empty())
			{
				issues.push_back(std::string{ asset_kind } + " archive path must not be empty");
			}
			if (asset_location.empty())
			{
				issues.push_back(std::string{ asset_kind } + " archive entry path must not be empty");
			}
		}

		void ValidateFilePathAssetSource(const std::string& asset_location,
			std::string_view asset_kind,
			bool require_existing_files,
			std::vector<std::string>& issues)
		{
			if (asset_location.empty())
			{
				issues.push_back(std::string{ asset_kind } + " asset path must not be empty");
			}
			else if (require_existing_files && !std::filesystem::exists(asset_location))
			{
				issues.push_back(std::string{ asset_kind } + " asset path does not exist: " + asset_location);
			}
		}

		void ValidateAssetSource(const AssetSource& source,
			std::string_view legacy_path,
			std::string_view asset_kind,
			bool require_existing_files,
			std::vector<std::string>& issues)
		{
			const std::string asset_location = AssetLocation(source, legacy_path);
			ValidateAssetHotReloadSource(source, asset_kind, issues);

			switch (source.storage)
			{
			case AssetStorage::EmbeddedData:
				ValidateEmbeddedAssetSource(source, asset_kind, issues);
				return;
			case AssetStorage::PackagedArchive:
				ValidatePackagedArchiveAssetSource(source, asset_location, asset_kind, issues);
				return;
			case AssetStorage::FilePath:
			default:
				ValidateFilePathAssetSource(asset_location, asset_kind, require_existing_files, issues);
				return;
			}
		}

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
		void ValidateAssets(const AssetRange& assets,
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

		void ValidateSearchRoots(std::span<const std::string> search_roots,
			bool require_existing_directories,
			std::vector<std::string>& issues)
		{
			for (const std::string& search_root : search_roots)
			{
				if (search_root.empty())
				{
					issues.push_back("asset search root must not be empty");
				}
				else if (require_existing_directories && !std::filesystem::is_directory(search_root))
				{
					issues.push_back("asset search root does not exist: " + search_root);
				}
			}
		}
	}
}
