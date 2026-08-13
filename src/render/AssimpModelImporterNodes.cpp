#include "AssimpModelImporterNodes.h"

#include <assimp/scene.h>

namespace
{
	glm::mat4 ReadNodeTransform(const aiMatrix4x4& value)
	{
		return {
			value.a1, value.b1, value.c1, value.d1,
			value.a2, value.b2, value.c2, value.d2,
			value.a3, value.b3, value.c3, value.d3,
			value.a4, value.b4, value.c4, value.d4
		};
	}
}

namespace ve::assets::detail
{
	ImportedNode ReadSceneNode(const aiNode& source_node)
	{
		ImportedNode imported_node{};
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
