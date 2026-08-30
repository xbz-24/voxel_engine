#include "AssimpModelImporter.h"

#include "CoreTypes.h"

#include <assimp/postprocess.h>
#include <algorithm>
#include <cctype>

namespace
{
	std::string LowerExtension(const std::filesystem::path& path)
	{
		std::string extension = path.extension().string();
		std::ranges::transform(extension, extension.begin(), [](unsigned char value) {
			return ve::core::ToChar(std::tolower(value));
		});
		return extension;
	}

	bool IsAssimpExtension(const std::string& extension)
	{
		static constexpr std::string_view supported[] = {
			".obj", ".fbx", ".gltf", ".glb", ".dae", ".stl", ".ply", ".3ds", ".blend", ".x"
		};
		return std::ranges::find(supported, extension) != std::end(supported);
	}
}

namespace ve::assets
{
	std::string_view AssimpModelImporter::Name() const noexcept
	{
		return "Assimp model importer";
	}

	bool AssimpModelImporter::CanImport(const std::filesystem::path& model_path) const
	{
		return IsAssimpExtension(LowerExtension(model_path));
	}

	unsigned int AssimpModelImporter::BuildPostProcessFlags(
		const ModelImportOptions& options) noexcept
	{
		unsigned int flags = aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality;
		if (options.triangulate) flags |= aiProcess_Triangulate;
		if (options.generate_normals) flags |= aiProcess_GenSmoothNormals;
		if (options.flip_uvs) flags |= aiProcess_FlipUVs;
		if (options.optimize_meshes) flags |= aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph;
		return flags;
	}
}
