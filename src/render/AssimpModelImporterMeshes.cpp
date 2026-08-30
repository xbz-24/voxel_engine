#include "AssimpModelImporter.h"

#include "AssimpModelImporterMeshDetail.h"

#include <assimp/mesh.h>

namespace ve::assets
{
	ImportedMesh AssimpModelImporter::ReadMesh(
		const aiMesh& mesh, const ModelImportOptions& options)
	{
		ImportedMesh imported_mesh;
		imported_mesh.name = mesh.mName.C_Str();
		imported_mesh.material_index = mesh.mMaterialIndex;
		imported_mesh.vertices.reserve(mesh.mNumVertices);
		for (unsigned int vertex_index = 0; vertex_index < mesh.mNumVertices; ++vertex_index)
		{
			ImportedVertex vertex = detail::ReadVertex(mesh, vertex_index);
			vertex.position *= options.unit_scale;
			imported_mesh.vertices.push_back(vertex);
		}
		detail::ReadJoints(mesh, imported_mesh);

		for (unsigned int face_index = 0; face_index < mesh.mNumFaces; ++face_index)
		{
			const aiFace& face = mesh.mFaces[face_index];
			for (unsigned int index = 0; index < face.mNumIndices; ++index)
			{
				imported_mesh.indices.push_back(face.mIndices[index]);
			}
		}
		return imported_mesh;
	}
}
