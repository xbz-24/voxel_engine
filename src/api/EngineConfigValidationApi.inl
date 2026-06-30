	std::vector<std::string> AssetCatalog::Validate(bool require_existing_files) const
	{
		std::vector<std::string> issues;
		ValidateSearchRoots(search_roots, require_existing_files, issues);
		ValidateAssets(textures, "texture", require_existing_files, issues);
		ValidateAssets(models, "model", require_existing_files, issues);
		ValidateAssets(sounds, "sound", require_existing_files, issues);
		return issues;
	}

	std::vector<std::string> MaterialLibrary::Validate() const
	{
		std::vector<std::string> issues;
		ValidateMaterials(materials, issues);
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
