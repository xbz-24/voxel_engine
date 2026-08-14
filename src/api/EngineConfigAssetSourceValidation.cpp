#include "EngineConfigAssetSourceValidation.h"
#include "EngineConfigFilesystemInspection.h"

#include <string>

namespace voxel::detail::config_validation
{
	namespace
	{
		[[nodiscard]] std::string AssetLocation(const AssetSource& source, std::string_view legacy_path)
		{
			return source.location.empty() ? std::string{ legacy_path } : source.location;
		}

		void ValidateHotReload(const AssetSource& source,
			std::string_view asset_kind,
			std::vector<std::string>& issues)
		{
			if (source.hot_reload && source.storage != AssetStorage::FilePath)
			{
				issues.push_back(std::string{ asset_kind } + " asset hot reload requires a filesystem path");
			}
		}

		void ValidateEmbedded(const AssetSource& source,
			std::string_view asset_kind,
			std::vector<std::string>& issues)
		{
			if (source.embedded_data.empty())
			{
				issues.push_back(std::string{ asset_kind } + " embedded asset data must not be empty");
			}
		}

		void ValidateArchive(const AssetSource& source,
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

		void ValidateFilePath(const std::string& asset_location,
			std::string_view asset_kind,
			bool require_existing_files,
			std::vector<std::string>& issues)
		{
			if (asset_location.empty())
			{
				issues.push_back(std::string{ asset_kind } + " asset path must not be empty");
			}
			else if (require_existing_files)
			{
				const FilesystemRequirementResult result = CheckPathExists(asset_location);
				if (result == FilesystemRequirementResult::InspectionFailed)
				{
					issues.push_back(std::string{ asset_kind } + " asset path could not be inspected: " + asset_location);
				}
				else if (result == FilesystemRequirementResult::NotSatisfied)
				{
					issues.push_back(std::string{ asset_kind } + " asset path does not exist: " + asset_location);
				}
			}
		}
	}

	void ValidateAssetSource(const AssetSource& source,
		std::string_view legacy_path,
		std::string_view asset_kind,
		bool require_existing_files,
		std::vector<std::string>& issues)
	{
		const std::string asset_location = AssetLocation(source, legacy_path);
		ValidateHotReload(source, asset_kind, issues);
		switch (source.storage)
		{
		case AssetStorage::EmbeddedData:
			return ValidateEmbedded(source, asset_kind, issues);
		case AssetStorage::PackagedArchive:
			return ValidateArchive(source, asset_location, asset_kind, issues);
		case AssetStorage::FilePath:
			return ValidateFilePath(asset_location, asset_kind, require_existing_files, issues);
		default:
			issues.push_back(std::string{ asset_kind } + " asset storage is not known");
			return;
		}
	}
}
