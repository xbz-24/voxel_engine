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

	/// Reads an Assimp matrix into GLM column-major storage.
	glm::mat4 ReadNodeTransform(const aiMatrix4x4& value)
	{
		return {
			value.a1, value.b1, value.c1, value.d1,
			value.a2, value.b2, value.c2, value.d2,
			value.a3, value.b3, value.c3, value.d3,
			value.a4, value.b4, value.c4, value.d4
		};
	}

	/// Copies source node hierarchy for tools that need authored transforms.
	ve::assets::ImportedNode ReadSceneNode(const aiNode& source_node)
	{
		ve::assets::ImportedNode imported_node{};
		imported_node.name = source_node.mName.C_Str();
		imported_node.transform = ReadNodeTransform(source_node.mTransformation);
		imported_node.mesh_indices.reserve(source_node.mNumMeshes);
		for (unsigned int mesh_index = 0; mesh_index < source_node.mNumMeshes; ++mesh_index)
		{
			imported_node.mesh_indices.push_back(source_node.mMeshes[mesh_index]);
		}
		imported_node.children.reserve(source_node.mNumChildren);
		for (unsigned int child_index = 0; child_index < source_node.mNumChildren; ++child_index)
		{
			imported_node.children.push_back(ReadSceneNode(*source_node.mChildren[child_index]));
		}
		return imported_node;
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
		if (scene->mRootNode != nullptr)
		{
			model.has_root_node = true;
			model.root_node = ReadSceneNode(*scene->mRootNode);
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
