#include "ModelImporterRegistry.h"

namespace ve::assets
{
	/// Registers one importer implementation.
	void ModelImporterRegistry::Register(std::unique_ptr<IModelImporter> importer)
	{
		if (importer) importers_.push_back(ve::core::Move(importer));
	}

	/// Finds the first importer that supports a model path.
	const IModelImporter* ModelImporterRegistry::FindImporter(const std::filesystem::path& model_path) const
	{
		for (const std::unique_ptr<IModelImporter>& importer : importers_)
		{
			if (importer->CanImport(model_path)) return importer.get();
		}
		return nullptr;
	}

	/// Imports a model through the matching registered importer.
	std::optional<ImportedModel> ModelImporterRegistry::Import(const std::filesystem::path& model_path, const ModelImportOptions& options) const
	{
		const IModelImporter* importer = FindImporter(model_path);
		return importer ? importer->Import(model_path, options) : std::nullopt;
	}

	/// Returns the number of registered importer implementations.
	ve::core::Index ModelImporterRegistry::ImporterCount() const noexcept
	{
		return importers_.size();
	}
}
