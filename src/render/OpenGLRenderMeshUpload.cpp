#include "OpenGLRenderMesh.h"

#include "OpenGLTypeConversions.h"

#include <cstdint>
#include <vector>

namespace ve::rendering
{
	void OpenGLRenderMesh::Upload(const MeshDescription& description)
	{
		Release();
		if (description.vertices.empty()) return;

		std::vector<std::uint32_t> indices;
		indexed_batches_.reserve(description.batches.size());
		for (const ChunkMeshBatch& source_batch : description.batches)
		{
			const std::uint32_t first_index = OpenGLMeshIndex(indices.size());
			for (std::uint32_t vertex_offset = 0;
				vertex_offset + 3U < source_batch.vertex_count; vertex_offset += 4U)
			{
				const std::uint32_t first_vertex = source_batch.first_vertex + vertex_offset;
				indices.insert(indices.end(), {
					first_vertex,
					first_vertex + 1U,
					first_vertex + 2U,
					first_vertex,
					first_vertex + 2U,
					first_vertex + 3U
				});
			}
			const std::uint32_t index_count = OpenGLMeshIndex(indices.size()) - first_index;
			if (index_count > 0U)
			{
				indexed_batches_.push_back(
					IndexedChunkMeshBatch{ source_batch.texture, first_index, index_count });
			}
		}

		glGenBuffers(1, &vertex_buffer_);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
		glBufferData(GL_ARRAY_BUFFER, OpenGLByteCount(description.vertices.size_bytes()),
			description.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &index_buffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			OpenGLByteCount(indices.size() * sizeof(std::uint32_t)),
			indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		vertex_count_ = OpenGLCount(description.vertices.size());
		index_count_ = OpenGLCount(indices.size());
	}
}
