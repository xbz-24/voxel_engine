#pragma once

#include "ModelImporter.h"

#include <memory>

namespace ve::assets
{
	class ModelImporterRegistry
	{
	public:
		/** @param importer Importer object owned by the registry. */
		void Register(std::unique_ptr<IModelImporter> importer);

		/** @param model_path Path to inspect. @return Importer pointer or null when unsupported. */
		[[nodiscard]] const IModelImporter* FindImporter(const std::filesystem::path& model_path) const;

		/** @param model_path Model path to load. @param options Import flags. @return Imported model or empty on failure. */
		[[nodiscard]] std::optional<ImportedModel> Import(const std::filesystem::path& model_path, const ModelImportOptions& options) const;

		/** @return Number of registered importer implementations. */
		[[nodiscard]] ve::core::Index ImporterCount() const noexcept;

	private:
		ve::core::DynamicArray<std::unique_ptr<IModelImporter>> importers_;
	};
}
