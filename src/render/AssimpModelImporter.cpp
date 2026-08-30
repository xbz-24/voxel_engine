#include "AssimpModelImporter.h"

#include "AssimpModelImporterNodes.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <utility>

namespace ve::assets
{
	/// Imports a model file into the engine-owned mesh/material format.
	std::optional<ImportedModel> AssimpModelImporter::Import(const std::filesystem::path& model_path, const ModelImportOptions& options) const
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(model_path.string(), BuildPostProcessFlags(options));
		if (!scene || !scene->HasMeshes()) return std::nullopt;
		ImportedModel model{ model_path };
		if (scene->mRootNode != nullptr)
		{
			model.has_root_node = true;
			model.root_node = detail::ReadSceneNode(*scene->mRootNode);
		}
		model.source_camera_count = scene->mNumCameras;
		model.source_light_count = scene->mNumLights;
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
		const auto report_missing_material_texture = [&model](const std::filesystem::path& texture_path) {
			if (!texture_path.empty() && !std::filesystem::exists(texture_path))
			{
				model.diagnostics.push_back(ModelImportDiagnostic{
					ModelImportDiagnosticSeverity::Warning,
					"Referenced material texture is missing: " + texture_path.string()
				});
			}
		};
		for (unsigned int index = 0; index < scene->mNumMaterials; index++)
		{
			ImportedMaterial material = ReadMaterial(*scene->mMaterials[index], model_path.parent_path());
			report_missing_material_texture(material.albedo_texture);
			report_missing_material_texture(material.normal_texture);
			report_missing_material_texture(material.metallic_roughness_texture);
			report_missing_material_texture(material.metallic_texture);
			report_missing_material_texture(material.roughness_texture);
			report_missing_material_texture(material.occlusion_texture);
			report_missing_material_texture(material.emissive_texture);
			model.materials.push_back(std::move(material));
		}
		for (unsigned int index = 0; index < scene->mNumMeshes; index++)
		{
			model.meshes.push_back(ReadMesh(*scene->mMeshes[index], options));
		}
		return model;
	}
}
