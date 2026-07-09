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

		[[nodiscard]] std::string MaterialLabel(const Material& material)
		{
			return material.name.empty() ? "material" : "material '" + material.name + "'";
		}

		[[nodiscard]] std::string EntityLabel(const Entity& entity)
		{
			return entity.name.empty() ? "scene entity" : "scene entity '" + entity.name + "'";
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

		void ValidateMaterialNameIsPresentAndUnique(const Material& material,
			std::set<std::string>& material_names_seen,
			std::vector<std::string>& issues)
		{
			if (material.name.empty())
			{
				issues.push_back("material name must not be empty");
			}
			else if (!material_names_seen.insert(material.name).second)
			{
				issues.push_back("material name is duplicated: " + material.name);
			}
		}

		void ValidateMaterialScalarProperties(const Material& material,
			const std::string& material_label,
			std::vector<std::string>& issues)
		{
			ValidateColor(material.base_color, material_label + ".base_color", issues);
			ValidateFiniteRange(material.metallic, material_label + ".metallic", NormalizedFloatRange, issues);
			ValidateFiniteRange(material.roughness, material_label + ".roughness", NormalizedFloatRange, issues);
			ValidateNonNegative(material.emission, material_label + ".emission", issues);
		}

		void ValidateMaterialDefinition(const Material& material,
			std::set<std::string>& material_names_seen,
			std::vector<std::string>& issues)
		{
			ValidateMaterialNameIsPresentAndUnique(material, material_names_seen, issues);

			const std::string material_label = MaterialLabel(material);
			ValidateMaterialScalarProperties(material, material_label, issues);
		}

		void ValidateMaterials(const std::vector<Material>& materials, std::vector<std::string>& issues)
		{
			std::set<std::string> material_names_seen;
			for (const Material& material : materials)
			{
				ValidateMaterialDefinition(material, material_names_seen, issues);
			}
		}

		template <std::ranges::input_range NamedRange>
		[[nodiscard]] std::set<std::string, std::less<>> CollectNonEmptyNamesFrom(const NamedRange& named_values)
		{
			std::set<std::string, std::less<>> non_empty_names;
			for (const auto& named_value : named_values)
			{
				if (!named_value.name.empty())
				{
					non_empty_names.insert(named_value.name);
				}
			}
			return non_empty_names;
		}

		void ValidateOptionalReference(std::string_view reference,
			const std::set<std::string, std::less<>>& known_names,
			const std::string& owner_label,
			std::string_view reference_kind,
			std::vector<std::string>& issues)
		{
			if (reference.empty() || known_names.contains(reference))
			{
				return;
			}
			issues.push_back(owner_label + " references missing " + std::string{ reference_kind } + ": " +
				std::string{ reference });
		}

		void ValidateMaterialTextureBindingReferences(const Material& material,
			const std::set<std::string, std::less<>>& texture_names,
			std::vector<std::string>& issues)
		{
			const std::string material_label = MaterialLabel(material);
			ValidateOptionalReference(material.texture, texture_names, material_label, "texture asset", issues);
			ValidateOptionalReference(material.normal_texture, texture_names, material_label, "texture asset", issues);
			ValidateOptionalReference(material.roughness_texture, texture_names, material_label, "texture asset", issues);
			ValidateOptionalReference(material.metallic_texture, texture_names, material_label, "texture asset", issues);
			ValidateOptionalReference(material.occlusion_texture, texture_names, material_label, "texture asset", issues);
			ValidateOptionalReference(material.emissive_texture, texture_names, material_label, "texture asset", issues);
		}

		void ValidateMaterialTextureReferences(
			const MaterialLibrary& materials,
			const AssetCatalog& assets,
			std::vector<std::string>& issues)
		{
			const std::set<std::string, std::less<>> texture_names = CollectNonEmptyNamesFrom(assets.textures);
			for (const Material& material : materials.materials)
			{
				ValidateMaterialTextureBindingReferences(material, texture_names, issues);
			}
		}

		void ValidateSceneGraphAssetReferences(
			const SceneGraph& scene_graph,
			const AssetCatalog& assets,
			const MaterialLibrary& materials,
			std::vector<std::string>& issues)
		{
			const std::set<std::string, std::less<>> model_names = CollectNonEmptyNamesFrom(assets.models);
			const std::set<std::string, std::less<>> material_names = CollectNonEmptyNamesFrom(materials.materials);
			for (const Entity& entity : scene_graph.entities)
			{
				const std::string entity_label = EntityLabel(entity);
				ValidateOptionalReference(entity.model, model_names, entity_label, "model asset", issues);
				ValidateOptionalReference(entity.material, material_names, entity_label, "material", issues);
			}
		}
	}
}
