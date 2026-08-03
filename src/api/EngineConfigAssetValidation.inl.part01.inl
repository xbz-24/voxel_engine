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
