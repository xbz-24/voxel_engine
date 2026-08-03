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
