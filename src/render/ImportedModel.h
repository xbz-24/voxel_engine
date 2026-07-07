#pragma once

#include "CoreTypes.h"

#include <cstdint>
#include <filesystem>
#include <glm/glm.hpp>
#include <string>

namespace ve::assets
{
	enum class ModelImportDiagnosticSeverity
	{
		Info,
		Warning,
		Error
	};

	struct ModelImportDiagnostic
	{
		ModelImportDiagnosticSeverity severity = ModelImportDiagnosticSeverity::Info;
		std::string message;
	};

	struct ImportedVertex
	{
		glm::vec3 position{ 0.0f };
		glm::vec3 normal{ 0.0f, 1.0f, 0.0f };
		glm::vec3 tangent{ 1.0f, 0.0f, 0.0f };
		glm::vec3 bitangent{ 0.0f, 1.0f, 0.0f };
		glm::vec2 texture_coordinates{ 0.0f };
		glm::vec4 color{ 1.0f };
		glm::uvec4 joint_indices{ 0U };
		glm::vec4 joint_weights{ 0.0f };
	};

	struct ImportedMaterial
	{
		std::string name;
		glm::vec4 base_color{ 1.0f };
		float metallic = 0.0f;
		float roughness = 0.8f;
		std::filesystem::path albedo_texture;
		std::filesystem::path normal_texture;
		std::filesystem::path metallic_roughness_texture;
		std::filesystem::path metallic_texture;
		std::filesystem::path roughness_texture;
		std::filesystem::path occlusion_texture;
		std::filesystem::path emissive_texture;
	};

	struct ImportedJoint
	{
		std::string name;
		glm::mat4 inverse_bind_transform{ 1.0f };
	};

	struct ImportedCustomVertexStream
	{
		std::string semantic;
		ve::core::DynamicArray<glm::vec4> values;
	};

	struct ImportedMesh
	{
		std::string name;
		ve::core::DynamicArray<ImportedVertex> vertices;
		ve::core::DynamicArray<std::uint32_t> indices;
		ve::core::DynamicArray<ImportedJoint> joints;
		ve::core::DynamicArray<ImportedCustomVertexStream> custom_vertex_streams;
		ve::core::Index material_index = 0;
	};

	struct ImportedNode
	{
		std::string name;
		glm::mat4 transform{ 1.0f };
		ve::core::DynamicArray<ve::core::Index> mesh_indices;
		ve::core::DynamicArray<ImportedNode> children;
	};

	struct ImportedModel
	{
		std::filesystem::path source_path;
		bool has_root_node = false;
		ImportedNode root_node{};
		ve::core::DynamicArray<ImportedMaterial> materials;
		ve::core::DynamicArray<ImportedMesh> meshes;
		ve::core::DynamicArray<ModelImportDiagnostic> diagnostics;
		std::uint32_t source_camera_count = 0;
		std::uint32_t source_light_count = 0;
		std::uint32_t source_animation_count = 0;
	};

	enum class ModelCoordinateSystem
	{
		Source,
		RightHandedYUp
	};

	enum class ModelMaterialImportPolicy
	{
		PbrPreferred,
		DiffuseOnly
	};

	struct ModelImportOptions
	{
		bool triangulate = true;
		bool generate_normals = true;
		bool flip_uvs = false;
		bool optimize_meshes = true;
		float unit_scale = 1.0f;
		ModelCoordinateSystem coordinate_system = ModelCoordinateSystem::Source;
		ModelMaterialImportPolicy material_policy = ModelMaterialImportPolicy::PbrPreferred;
	};
}
