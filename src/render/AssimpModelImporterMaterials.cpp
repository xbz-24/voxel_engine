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

	/// Reads the first available texture path for the requested Assimp slots.
	std::filesystem::path ReadTexture(
		const aiMaterial& material,
		const std::filesystem::path& model_directory,
		aiTextureType primary_type,
		aiTextureType fallback_type = aiTextureType_NONE)
	{
		aiString texture_path;
		if (material.GetTexture(primary_type, 0, &texture_path) == AI_SUCCESS)
		{
			return model_directory / texture_path.C_Str();
		}
		if (fallback_type != aiTextureType_NONE && material.GetTexture(fallback_type, 0, &texture_path) == AI_SUCCESS)
		{
			return model_directory / texture_path.C_Str();
		}
		return {};
	}
}

namespace ve::assets
{
	/// Converts one Assimp material into the engine material DTO.
	ImportedMaterial AssimpModelImporter::ReadMaterial(const aiMaterial& material, const std::filesystem::path& model_directory)
	{
		aiString name;
		material.Get(AI_MATKEY_NAME, name);
		ImportedMaterial imported_material{};
		imported_material.name = name.C_Str();
		imported_material.base_color = ReadBaseColor(material);
		imported_material.metallic = ReadScalar(material, AI_MATKEY_METALLIC_FACTOR, 0.0f);
		imported_material.roughness = ReadScalar(material, AI_MATKEY_ROUGHNESS_FACTOR, 0.8f);
		imported_material.albedo_texture = ReadTexture(material, model_directory, aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE);
		imported_material.normal_texture = ReadTexture(material, model_directory, aiTextureType_NORMAL_CAMERA, aiTextureType_NORMALS);
		imported_material.metallic_roughness_texture = ReadTexture(material, model_directory, aiTextureType_GLTF_METALLIC_ROUGHNESS);
		imported_material.metallic_texture = ReadTexture(material, model_directory, aiTextureType_METALNESS);
		imported_material.roughness_texture = ReadTexture(material, model_directory, aiTextureType_DIFFUSE_ROUGHNESS);
		imported_material.occlusion_texture = ReadTexture(material, model_directory, aiTextureType_AMBIENT_OCCLUSION);
		imported_material.emissive_texture = ReadTexture(material, model_directory, aiTextureType_EMISSION_COLOR, aiTextureType_EMISSIVE);
		return imported_material;
	}
}
