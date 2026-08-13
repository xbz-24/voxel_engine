#include "AssimpModelImporterMeshDetail.h"

#include <assimp/mesh.h>

namespace
{
	glm::mat4 ReadMatrix(const aiMatrix4x4& value)
	{
		return {
			value.a1, value.b1, value.c1, value.d1,
			value.a2, value.b2, value.c2, value.d2,
			value.a3, value.b3, value.c3, value.d3,
			value.a4, value.b4, value.c4, value.d4
		};
	}

	void AddJointInfluence(
		ve::assets::ImportedVertex& vertex, std::uint32_t joint_index, float weight) noexcept
	{
		if (weight <= 0.0f) return;
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

	void NormalizeJointWeights(ve::assets::ImportedVertex& vertex) noexcept
	{
		const float total_weight = vertex.joint_weights.x + vertex.joint_weights.y +
			vertex.joint_weights.z + vertex.joint_weights.w;
		if (total_weight > 0.0f) vertex.joint_weights /= total_weight;
	}
}

namespace ve::assets::detail
{
	void ReadJoints(const aiMesh& mesh, ImportedMesh& imported_mesh)
	{
		imported_mesh.joints.reserve(mesh.mNumBones);
		for (unsigned int bone_index = 0; bone_index < mesh.mNumBones; ++bone_index)
		{
			const aiBone& bone = *mesh.mBones[bone_index];
			imported_mesh.joints.push_back(
				ImportedJoint{ bone.mName.C_Str(), ReadMatrix(bone.mOffsetMatrix) });
			for (unsigned int weight_index = 0; weight_index < bone.mNumWeights; ++weight_index)
			{
				const aiVertexWeight& source_weight = bone.mWeights[weight_index];
				if (source_weight.mVertexId < imported_mesh.vertices.size())
				{
					AddJointInfluence(imported_mesh.vertices[source_weight.mVertexId],
						bone_index, source_weight.mWeight);
				}
			}
		}
		for (ImportedVertex& vertex : imported_mesh.vertices) NormalizeJointWeights(vertex);
	}
}
