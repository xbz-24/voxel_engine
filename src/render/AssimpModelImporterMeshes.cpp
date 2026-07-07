#include "AssimpModelImporter.h"

#include <assimp/mesh.h>

namespace
{
	/// Reads a vertex position.
	glm::vec3 ReadVector3(const aiVector3D& value)
	{
		return { value.x, value.y, value.z };
	}

	/// Reads an Assimp matrix into GLM column-major storage.
	glm::mat4 ReadMatrix(const aiMatrix4x4& value)
	{
		return {
			value.a1, value.b1, value.c1, value.d1,
			value.a2, value.b2, value.c2, value.d2,
			value.a3, value.b3, value.c3, value.d3,
			value.a4, value.b4, value.c4, value.d4
		};
	}

	/// Reads the first vertex color set or returns opaque white when missing.
	glm::vec4 ReadVertexColor(const aiMesh& mesh, unsigned int vertex_index)
	{
		if (!mesh.HasVertexColors(0)) return { 1.0f, 1.0f, 1.0f, 1.0f };
		const aiColor4D& color = mesh.mColors[0][vertex_index];
		return { color.r, color.g, color.b, color.a };
	}

	/// Reads texture coordinates or returns zeroes when missing.
	glm::vec2 ReadTexCoord(const aiMesh& mesh, unsigned int vertex_index)
	{
		if (!mesh.HasTextureCoords(0)) return { 0.0f, 0.0f };
		const aiVector3D& value = mesh.mTextureCoords[0][vertex_index];
		return { value.x, value.y };
	}

	/// Stores the strongest four joint influences on a vertex.
	void AddJointInfluence(ve::assets::ImportedVertex& vertex, std::uint32_t joint_index, float weight) noexcept
	{
		if (weight <= 0.0f)
		{
			return;
		}

		for (int slot = 0; slot < 4; ++slot)
		{
			if (vertex.joint_weights[slot] == 0.0f)
			{
				vertex.joint_indices[slot] = joint_index;
				vertex.joint_weights[slot] = weight;
				return;
			}
		}

		int weakest_slot = 0;
		for (int slot = 1; slot < 4; ++slot)
		{
			if (vertex.joint_weights[slot] < vertex.joint_weights[weakest_slot])
			{
				weakest_slot = slot;
			}
		}
		if (weight > vertex.joint_weights[weakest_slot])
		{
			vertex.joint_indices[weakest_slot] = joint_index;
			vertex.joint_weights[weakest_slot] = weight;
		}
	}

	/// Normalizes joint weights after all source bones have been read.
	void NormalizeJointWeights(ve::assets::ImportedVertex& vertex) noexcept
	{
		const float total_weight = vertex.joint_weights.x + vertex.joint_weights.y +
			vertex.joint_weights.z + vertex.joint_weights.w;
		if (total_weight > 0.0f)
		{
			vertex.joint_weights /= total_weight;
		}
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
			ImportedVertex vertex{};
			vertex.position = ReadVector3(mesh.mVertices[vertex_index]) * options.unit_scale;
			vertex.normal = mesh.HasNormals() ? ReadVector3(mesh.mNormals[vertex_index]) : glm::vec3(0.0f, 1.0f, 0.0f);
			if (mesh.HasTangentsAndBitangents())
			{
				vertex.tangent = ReadVector3(mesh.mTangents[vertex_index]);
				vertex.bitangent = ReadVector3(mesh.mBitangents[vertex_index]);
			}
			vertex.texture_coordinates = ReadTexCoord(mesh, vertex_index);
			vertex.color = ReadVertexColor(mesh, vertex_index);
			imported_mesh.vertices.push_back(vertex);
		}
		imported_mesh.joints.reserve(mesh.mNumBones);
		for (unsigned int bone_index = 0; bone_index < mesh.mNumBones; ++bone_index)
		{
			const aiBone& bone = *mesh.mBones[bone_index];
			ImportedJoint joint{};
			joint.name = bone.mName.C_Str();
			joint.inverse_bind_transform = ReadMatrix(bone.mOffsetMatrix);
			imported_mesh.joints.push_back(joint);

			for (unsigned int weight_index = 0; weight_index < bone.mNumWeights; ++weight_index)
			{
				const aiVertexWeight& source_weight = bone.mWeights[weight_index];
				if (source_weight.mVertexId < imported_mesh.vertices.size())
				{
					AddJointInfluence(imported_mesh.vertices[source_weight.mVertexId], bone_index, source_weight.mWeight);
				}
			}
		}
		for (ImportedVertex& vertex : imported_mesh.vertices)
		{
			NormalizeJointWeights(vertex);
		}
		for (unsigned int face_index = 0; face_index < mesh.mNumFaces; face_index++)
		{
			const aiFace& face = mesh.mFaces[face_index];
			for (unsigned int index = 0; index < face.mNumIndices; index++) imported_mesh.indices.push_back(face.mIndices[index]);
		}
		return imported_mesh;
	}
}
