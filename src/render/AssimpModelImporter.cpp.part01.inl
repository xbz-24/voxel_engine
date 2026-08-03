#include "AssimpModelImporter.h"

#include "CoreTypes.h"

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
		std::ranges::transform(extension, extension.begin(), [](unsigned char value) { return ve::core::ToChar(std::tolower(value)); });
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
