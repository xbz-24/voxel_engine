namespace voxel
{
	namespace
	{
		[[nodiscard]] std::string AssetLocation(const AssetSource& source, const std::string& legacy_path)
		{
			return source.location.empty() ? legacy_path : source.location;
		}

		void ValidateAssetSource(const AssetSource& source,
			const std::string& legacy_path,
			std::string_view kind,
			bool require_existing_files,
			std::vector<std::string>& issues)
		{
			const std::string location = AssetLocation(source, legacy_path);
			if (source.hot_reload && source.storage != AssetStorage::FilePath)
			{
				issues.push_back(std::string{ kind } + " asset hot reload requires a filesystem path");
			}

			switch (source.storage)
			{
			case AssetStorage::EmbeddedData:
				if (source.embedded_data.empty())
				{
					issues.push_back(std::string{ kind } + " embedded asset data must not be empty");
				}
				return;
			case AssetStorage::PackagedArchive:
				if (source.archive_path.empty())
				{
					issues.push_back(std::string{ kind } + " archive path must not be empty");
				}
				if (location.empty())
				{
					issues.push_back(std::string{ kind } + " archive entry path must not be empty");
				}
				return;
			case AssetStorage::FilePath:
			default:
				if (location.empty())
				{
					issues.push_back(std::string{ kind } + " asset path must not be empty");
				}
				else if (require_existing_files && !std::filesystem::exists(location))
				{
					issues.push_back(std::string{ kind } + " asset path does not exist: " + location);
				}
				return;
			}
		}

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
				ValidateAssetSource(asset.source, asset.path, kind, require_existing_files, issues);
			}
		}

		void ValidateSearchRoots(const std::vector<std::string>& search_roots,
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

		[[nodiscard]] std::string MaterialLabel(const Material& material)
		{
			return material.name.empty() ? "material" : "material '" + material.name + "'";
		}

		void ValidateFiniteRange(float value,
			std::string label,
			FloatRange range,
			std::vector<std::string>& issues)
		{
			if (!std::isfinite(value))
			{
				issues.push_back(label + " must be finite");
			}
			else if (!range.Contains(value))
			{
				issues.push_back(label + " must be between 0 and 1");
			}
		}

		void ValidateNonNegative(float value, std::string label, std::vector<std::string>& issues)
		{
			if (!std::isfinite(value))
			{
				issues.push_back(label + " must be finite");
			}
			else if (value < MinimumEmissionStrength)
			{
				issues.push_back(label + " must be zero or greater");
			}
		}

		void ValidateColor(Color color, const std::string& label, std::vector<std::string>& issues)
		{
			ValidateFiniteRange(color.r, label + ".r", NormalizedFloatRange, issues);
			ValidateFiniteRange(color.g, label + ".g", NormalizedFloatRange, issues);
			ValidateFiniteRange(color.b, label + ".b", NormalizedFloatRange, issues);
			ValidateFiniteRange(color.a, label + ".a", NormalizedFloatRange, issues);
		}

		void ValidateMaterials(const std::vector<Material>& materials, std::vector<std::string>& issues)
		{
			std::set<std::string> names;
			for (const Material& material : materials)
			{
				if (material.name.empty())
				{
					issues.push_back("material name must not be empty");
				}
				else if (!names.insert(material.name).second)
				{
					issues.push_back("material name is duplicated: " + material.name);
				}

				const std::string material_label = MaterialLabel(material);
				ValidateColor(material.base_color, material_label + ".base_color", issues);
				ValidateFiniteRange(material.metallic, material_label + ".metallic", NormalizedFloatRange, issues);
				ValidateFiniteRange(material.roughness, material_label + ".roughness", NormalizedFloatRange, issues);
				ValidateNonNegative(material.emission, material_label + ".emission", issues);
			}
		}
	}
}
