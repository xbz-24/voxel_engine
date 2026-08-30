#include "AssimpModelImporterMeshDetail.h"

#include <assimp/mesh.h>

namespace
{
	glm::vec3 ReadVector3(const aiVector3D& value)
	{
		return { value.x, value.y, value.z };
	}

	glm::vec4 ReadVertexColor(const aiMesh& mesh, unsigned int vertex_index)
	{
		if (!mesh.HasVertexColors(0)) return { 1.0f, 1.0f, 1.0f, 1.0f };
		const aiColor4D& color = mesh.mColors[0][vertex_index];
		return { color.r, color.g, color.b, color.a };
	}

	glm::vec2 ReadTexCoord(const aiMesh& mesh, unsigned int vertex_index)
	{
		if (!mesh.HasTextureCoords(0)) return { 0.0f, 0.0f };
		const aiVector3D& value = mesh.mTextureCoords[0][vertex_index];
		return { value.x, value.y };
	}
}

namespace ve::assets::detail
{
	ImportedVertex ReadVertex(const aiMesh& mesh, unsigned int vertex_index)
	{
		ImportedVertex vertex{};
		vertex.position = ReadVector3(mesh.mVertices[vertex_index]);
		vertex.normal = mesh.HasNormals()
			? ReadVector3(mesh.mNormals[vertex_index])
			: glm::vec3(0.0f, 1.0f, 0.0f);
		if (mesh.HasTangentsAndBitangents())
		{
			vertex.tangent = ReadVector3(mesh.mTangents[vertex_index]);
			vertex.bitangent = ReadVector3(mesh.mBitangents[vertex_index]);
		}
		vertex.texture_coordinates = ReadTexCoord(mesh, vertex_index);
		vertex.color = ReadVertexColor(mesh, vertex_index);
		return vertex;
	}
}
