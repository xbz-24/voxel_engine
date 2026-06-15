#pragma once

#include "CoreTypes.h"

#include <cstdint>
#include <filesystem>
#include <glm/glm.hpp>
#include <string>

namespace ve::assets
{
	struct ImportedVertex
	{
		glm::vec3 position{ 0.0f };
		glm::vec3 normal{ 0.0f, 1.0f, 0.0f };
		glm::vec2 texture_coordinates{ 0.0f };
	};

	struct ImportedMaterial
	{
		std::string name;
		glm::vec4 base_color{ 1.0f };
		float metallic = 0.0f;
		float roughness = 0.8f;
		std::filesystem::path albedo_texture;
	};

	struct ImportedMesh
	{
		std::string name;
		ve::core::DynamicArray<ImportedVertex> vertices;
		ve::core::DynamicArray<std::uint32_t> indices;
		ve::core::Index material_index = 0;
	};

	struct ImportedModel
	{
		std::filesystem::path source_path;
		ve::core::DynamicArray<ImportedMaterial> materials;
		ve::core::DynamicArray<ImportedMesh> meshes;
	};

	struct ModelImportOptions
	{
		bool triangulate = true;
		bool generate_normals = true;
		bool flip_uvs = false;
		bool optimize_meshes = true;
	};
}
