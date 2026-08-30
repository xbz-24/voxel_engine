#pragma once

#include "CoreTypes.h"

#include <cstdint>
#include <glm/glm.hpp>
#include <string>

namespace ve::assets
{
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
}
