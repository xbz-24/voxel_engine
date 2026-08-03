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
