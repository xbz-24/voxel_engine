#include "ModelAssetLibrary.h"

#include "AssimpModelImporter.h"

namespace ve::assets
{
	/// Creates a model library with built-in importers registered.
	ModelAssetLibrary::ModelAssetLibrary()
	{
		RegisterImporter(std::make_unique<AssimpModelImporter>());
	}

	/// Imports one model using a registered importer.
	std::optional<ImportedModel> ModelAssetLibrary::ImportModel(const std::filesystem::path& model_path, const ModelImportOptions& options) const
	{
		return importers_.Import(model_path, options);
	}

	/// Registers a custom importer for proprietary engine/game formats.
	void ModelAssetLibrary::RegisterImporter(std::unique_ptr<IModelImporter> importer)
	{
		importers_.Register(ve::core::Move(importer));
	}

	/// Returns the number of importer implementations available.
	ve::core::Index ModelAssetLibrary::ImporterCount() const noexcept
	{
		return importers_.ImporterCount();
	}
}
