#include "EngineRuntimeBridge.h"
#include "EngineConfigValidationInternal.h"

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
std::vector<std::string> AssetCatalog::Validate(bool require_existing_files) const
	{
		std::vector<std::string> issues;
		detail::config_validation::ValidateSearchRoots(search_roots, require_existing_files, issues);
		detail::config_validation::ValidateAssets(textures, "texture", require_existing_files, issues);
		detail::config_validation::ValidateAssets(models, "model", require_existing_files, issues);
		detail::config_validation::ValidateAssets(sounds, "sound", require_existing_files, issues);
		return issues;
	}

	std::vector<std::string> MaterialLibrary::Validate() const
	{
		std::vector<std::string> issues;
		detail::config_validation::ValidateMaterials(materials, issues);
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
