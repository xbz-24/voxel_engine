#pragma once

#include "ModelImporterRegistry.h"

namespace ve::assets
{
	class ModelAssetLibrary
	{
	public:
		/** Creates a model library with built-in importers registered. */
		ModelAssetLibrary();

		/** @param model_path Model path to load. @param options Import flags. @return Imported model or empty on failure. */
		[[nodiscard]] std::optional<ImportedModel> ImportModel(const std::filesystem::path& model_path, const ModelImportOptions& options = {}) const;

		/** @param importer Custom importer for proprietary engine/game formats. */
		void RegisterImporter(std::unique_ptr<IModelImporter> importer);

		/** @return Number of importer implementations available. */
		[[nodiscard]] ve::core::Index ImporterCount() const noexcept;

	private:
		ModelImporterRegistry importers_;
	};
}
