#include "ModelAssetLibrary.h"

#include "AssimpModelImporter.h"

#include <sstream>

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
		const std::string key = CacheKey(model_path, options);
		if (const auto cached = cache_.find(key); cached != cache_.end())
		{
			return cached->second;
		}

		std::optional<ImportedModel> imported = importers_.Import(model_path, options);
		if (imported)
		{
			cache_.emplace(key, *imported);
		}
		return imported;
	}

	void ModelAssetLibrary::ClearCache() const
	{
		cache_.clear();
	}

	/// Registers a custom importer for proprietary engine/game formats.
	void ModelAssetLibrary::RegisterImporter(std::unique_ptr<IModelImporter> importer)
	{
		importers_.Register(ve::core::Move(importer));
		ClearCache();
	}

	/// Returns the number of importer implementations available.
	ve::core::Index ModelAssetLibrary::ImporterCount() const noexcept
	{
		return importers_.ImporterCount();
	}

	std::string ModelAssetLibrary::CacheKey(const std::filesystem::path& model_path, const ModelImportOptions& options)
	{
		std::ostringstream stream;
		stream << model_path.lexically_normal().generic_string()
			<< "|triangulate=" << options.triangulate
			<< "|generate_normals=" << options.generate_normals
			<< "|flip_uvs=" << options.flip_uvs
			<< "|optimize_meshes=" << options.optimize_meshes;
		return stream.str();
	}
}
