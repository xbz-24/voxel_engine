#include "ChunkMeshAssembler.h"

#include "ChunkFaceEmitter.h"
#include "CoreTypes.h"

#include <algorithm>

namespace ve::world::mesh::detail
{
	namespace
	{
		void AppendBatch(
			ChunkMeshBuildResult& result,
			ve::rendering::TextureHandle texture,
			std::uint32_t first_vertex)
		{
			const std::uint32_t vertex_count =
				ve::core::ToU32(result.vertices.size()) - first_vertex;
			if (vertex_count > 0)
			{
				result.batches.push_back({ texture, first_vertex, vertex_count });
			}
		}
	}

	ChunkMeshBuildResult AssembleChunkMesh(
		std::vector<MeshFace> faces,
		ChunkMeshBuildDiagnostics diagnostics)
	{
		std::sort(faces.begin(), faces.end(), [](const MeshFace& left, const MeshFace& right)
		{
			return left.material.texture < right.material.texture;
		});
		ChunkMeshBuildResult result;
		result.diagnostics = diagnostics;
		result.vertices.reserve(faces.size() * 4);
		if (faces.empty()) return result;

		ve::rendering::TextureHandle current_texture = faces.front().material.texture;
		std::uint32_t batch_start = 0;
		for (const MeshFace& face : faces)
		{
			if (face.material.texture != current_texture)
			{
				AppendBatch(result, current_texture, batch_start);
				current_texture = face.material.texture;
				batch_start = ve::core::ToU32(result.vertices.size());
			}
			AppendFaceVertices(face, result.vertices);
		}
		AppendBatch(result, current_texture, batch_start);
		result.diagnostics.vertex_count = result.vertices.size();
		result.diagnostics.batch_count = result.batches.size();
		return result;
	}
}
