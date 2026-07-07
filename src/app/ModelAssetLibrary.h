#pragma once

#include "ModelImporterRegistry.h"

#include <string>
#include <unordered_map>

namespace ve::assets
{
	class ModelAssetLibrary
	{
	public:
		/** Creates a model library with built-in importers registered. */
		ModelAssetLibrary();

		/** @param model_path Model path to load. @param options Import flags. @return Imported model or empty on failure. */
		[[nodiscard]] std::optional<ImportedModel> ImportModel(const std::filesystem::path& model_path, const ModelImportOptions& options = {}) const;

		/** Clears cached imported models. */
		void ClearCache() const;

		/** @param importer Custom importer for proprietary engine/game formats. */
		void RegisterImporter(std::unique_ptr<IModelImporter> importer);

		/** @return Number of importer implementations available. */
		[[nodiscard]] ve::core::Index ImporterCount() const noexcept;

	private:
		[[nodiscard]] static std::string CacheKey(const std::filesystem::path& model_path, const ModelImportOptions& options);

		// Private importer registry until public AssetCatalog runtime loading supports custom formats.
		ModelImporterRegistry importers_;
		mutable std::unordered_map<std::string, ImportedModel> cache_;
	};
}
