#include "AssimpModelImporter.h"

#include <assimp/material.h>
#include <assimp/scene.h>

namespace
{
	/// Reads a color with fallback to white.
	glm::vec4 ReadBaseColor(const aiMaterial& material)
	{
		aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
		if (AI_SUCCESS != aiGetMaterialColor(&material, AI_MATKEY_BASE_COLOR, &color))
		{
			aiGetMaterialColor(&material, AI_MATKEY_COLOR_DIFFUSE, &color);
		}
		return { color.r, color.g, color.b, color.a };
	}

	/// Reads a scalar material property with a fallback value.
	float ReadScalar(const aiMaterial& material, const char* key, unsigned int type, unsigned int index, float fallback)
	{
		float value = fallback;
		material.Get(key, type, index, value);
		return value;
	}

	/// Reads the first available albedo texture path.
	std::filesystem::path ReadAlbedoTexture(const aiMaterial& material, const std::filesystem::path& model_directory)
	{
		aiString texture_path;
		if (material.GetTexture(aiTextureType_BASE_COLOR, 0, &texture_path) != AI_SUCCESS &&
			material.GetTexture(aiTextureType_DIFFUSE, 0, &texture_path) != AI_SUCCESS) return {};
		return model_directory / texture_path.C_Str();
	}
}

namespace ve::assets
{
	/// Converts one Assimp material into the engine material DTO.
	ImportedMaterial AssimpModelImporter::ReadMaterial(const aiMaterial& material, const std::filesystem::path& model_directory)
	{
		aiString name;
		material.Get(AI_MATKEY_NAME, name);
		return ImportedMaterial{
			name.C_Str(),
			ReadBaseColor(material),
			ReadScalar(material, AI_MATKEY_METALLIC_FACTOR, 0.0f),
			ReadScalar(material, AI_MATKEY_ROUGHNESS_FACTOR, 0.8f),
			ReadAlbedoTexture(material, model_directory)
		};
	}
}
