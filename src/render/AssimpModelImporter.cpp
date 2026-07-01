#include "AssimpModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <algorithm>
#include <cctype>
#include <utility>

namespace
{
	/// Converts a file extension to lowercase text.
	std::string LowerExtension(const std::filesystem::path& path)
	{
		std::string extension = path.extension().string();
		std::ranges::transform(extension, extension.begin(), [](unsigned char value) { return static_cast<char>(std::tolower(value)); });
		return extension;
	}

	/// Reports whether the extension is covered by the Assimp-backed importer.
	bool IsAssimpExtension(const std::string& extension)
	{
		static constexpr std::string_view supported[] = { ".obj", ".fbx", ".gltf", ".glb", ".dae", ".stl", ".ply", ".3ds", ".blend", ".x" };
		for (const std::string_view candidate : supported)
		{
			if (candidate == extension) return true;
		}
		return false;
	}
}

namespace ve::assets
{
	/// Returns the importer name.
	std::string_view AssimpModelImporter::Name() const noexcept { return "Assimp model importer"; }

	/// Reports whether Assimp supports the extension.
	bool AssimpModelImporter::CanImport(const std::filesystem::path& model_path) const
	{
		return IsAssimpExtension(LowerExtension(model_path));
	}

	/// Converts engine import options into Assimp post-process flags.
	unsigned int AssimpModelImporter::BuildPostProcessFlags(const ModelImportOptions& options) noexcept
	{
		unsigned int flags = aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality;
		if (options.triangulate) flags |= aiProcess_Triangulate;
		if (options.generate_normals) flags |= aiProcess_GenSmoothNormals;
		if (options.flip_uvs) flags |= aiProcess_FlipUVs;
		if (options.optimize_meshes) flags |= aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph;
		return flags;
	}

	/// Imports a model file into the engine-owned mesh/material format.
	std::optional<ImportedModel> AssimpModelImporter::Import(const std::filesystem::path& model_path, const ModelImportOptions& options) const
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(model_path.string(), BuildPostProcessFlags(options));
		if (!scene || !scene->HasMeshes()) return std::nullopt;
		ImportedModel model{ model_path };
		model.source_animation_count = scene->mNumAnimations;
		if (model.source_animation_count > 0)
		{
			model.diagnostics.push_back(ModelImportDiagnostic{
				ModelImportDiagnosticSeverity::Warning,
				"Source model contains animations; animation curves are counted but not imported into runtime clips yet."
			});
		}
		if (options.unit_scale != 1.0f)
		{
			model.diagnostics.push_back(ModelImportDiagnostic{
				ModelImportDiagnosticSeverity::Info,
				"Applied unit scale to imported vertex positions."
			});
		}
		for (unsigned int index = 0; index < scene->mNumMaterials; index++)
		{
			ImportedMaterial material = ReadMaterial(*scene->mMaterials[index], model_path.parent_path());
			if (!material.albedo_texture.empty() && !std::filesystem::exists(material.albedo_texture))
			{
				model.diagnostics.push_back(ModelImportDiagnostic{
					ModelImportDiagnosticSeverity::Warning,
					"Referenced material texture is missing: " + material.albedo_texture.string()
				});
			}
			model.materials.push_back(std::move(material));
		}
		for (unsigned int index = 0; index < scene->mNumMeshes; index++)
		{
			model.meshes.push_back(ReadMesh(*scene->mMeshes[index], options));
		}
		return model;
	}
}
