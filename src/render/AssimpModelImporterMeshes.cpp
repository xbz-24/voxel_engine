#include "AssimpModelImporter.h"

#include <assimp/mesh.h>

namespace
{
	/// Reads a vertex position.
	glm::vec3 ReadVector3(const aiVector3D& value)
	{
		return { value.x, value.y, value.z };
	}

	/// Reads texture coordinates or returns zeroes when missing.
	glm::vec2 ReadTexCoord(const aiMesh& mesh, unsigned int vertex_index)
	{
		if (!mesh.HasTextureCoords(0)) return { 0.0f, 0.0f };
		const aiVector3D& value = mesh.mTextureCoords[0][vertex_index];
		return { value.x, value.y };
	}
}

namespace ve::assets
{
	/// Converts one Assimp mesh into engine-owned vertex/index data.
	ImportedMesh AssimpModelImporter::ReadMesh(const aiMesh& mesh, const ModelImportOptions& options)
	{
		ImportedMesh imported_mesh;
		imported_mesh.name = mesh.mName.C_Str();
		imported_mesh.material_index = mesh.mMaterialIndex;
		imported_mesh.vertices.reserve(mesh.mNumVertices);
		for (unsigned int vertex_index = 0; vertex_index < mesh.mNumVertices; vertex_index++)
		{
			imported_mesh.vertices.push_back(ImportedVertex{
				ReadVector3(mesh.mVertices[vertex_index]) * options.unit_scale,
				mesh.HasNormals() ? ReadVector3(mesh.mNormals[vertex_index]) : glm::vec3(0.0f, 1.0f, 0.0f),
				ReadTexCoord(mesh, vertex_index)
			});
		}
		for (unsigned int face_index = 0; face_index < mesh.mNumFaces; face_index++)
		{
			const aiFace& face = mesh.mFaces[face_index];
			for (unsigned int index = 0; index < face.mNumIndices; index++) imported_mesh.indices.push_back(face.mIndices[index]);
		}
		return imported_mesh;
	}
}
