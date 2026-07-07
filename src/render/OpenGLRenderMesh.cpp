#include "RenderMesh.h"

#include "TextureLoader.h"

#include <GL/glew.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ve::rendering
{
	namespace
	{
		class OpenGLRenderMesh final : public RenderMesh
		{
		public:
			OpenGLRenderMesh() = default;
			~OpenGLRenderMesh() override { Release(); }

			OpenGLRenderMesh(const OpenGLRenderMesh&) = delete;
			OpenGLRenderMesh& operator=(const OpenGLRenderMesh&) = delete;

			void Upload(const MeshDescription& description) override
			{
				Release();
				if (description.vertices.empty()) return;

				std::vector<std::uint32_t> indices;
				indexed_batches_.reserve(description.batches.size());
				for (const ChunkMeshBatch& source_batch : description.batches)
				{
					const std::uint32_t first_index = static_cast<std::uint32_t>(indices.size());
					for (std::uint32_t vertex_offset = 0; vertex_offset + 3U < source_batch.vertex_count; vertex_offset += 4U)
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
					const std::uint32_t index_count = static_cast<std::uint32_t>(indices.size()) - first_index;
					if (index_count > 0U)
					{
						indexed_batches_.push_back(IndexedChunkMeshBatch{ source_batch.texture, first_index, index_count });
					}
				}

				glGenBuffers(1, &vertex_buffer_);
				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
				glBufferData(GL_ARRAY_BUFFER,
					static_cast<GLsizeiptr>(description.vertices.size_bytes()),
					description.vertices.data(),
					GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				glGenBuffers(1, &index_buffer_);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					static_cast<GLsizeiptr>(indices.size() * sizeof(std::uint32_t)),
					indices.data(),
					GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				vertex_count_ = static_cast<GLsizei>(description.vertices.size());
				index_count_ = static_cast<GLsizei>(indices.size());
			}

			void Draw() const override
			{
				if (vertex_buffer_ == 0 || index_buffer_ == 0 || vertex_count_ == 0 || index_count_ == 0) return;

				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);
				glVertexPointer(3, GL_FLOAT, sizeof(ChunkVertex), reinterpret_cast<void*>(offsetof(ChunkVertex, x)));
				glTexCoordPointer(2, GL_FLOAT, sizeof(ChunkVertex), reinterpret_cast<void*>(offsetof(ChunkVertex, u)));
				glColorPointer(3, GL_FLOAT, sizeof(ChunkVertex), reinterpret_cast<void*>(offsetof(ChunkVertex, r)));

				for (const IndexedChunkMeshBatch& batch : indexed_batches_)
				{
					glBindTexture(GL_TEXTURE_2D, NativeOpenGLTexture(batch.texture));
					const auto* first_index_offset =
						reinterpret_cast<const void*>(static_cast<std::uintptr_t>(batch.first_index) * sizeof(std::uint32_t));
					glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(batch.index_count), GL_UNSIGNED_INT, first_index_offset);
				}

				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glColor3f(1.0f, 1.0f, 1.0f);
			}

			void Release() override
			{
				if (vertex_buffer_ != 0)
				{
					glDeleteBuffers(1, &vertex_buffer_);
					vertex_buffer_ = 0;
				}
				if (index_buffer_ != 0)
				{
					glDeleteBuffers(1, &index_buffer_);
					index_buffer_ = 0;
				}
				vertex_count_ = 0;
				index_count_ = 0;
				indexed_batches_.clear();
			}

		private:
			struct IndexedChunkMeshBatch
			{
				TextureHandle texture = kInvalidTextureHandle;
				std::uint32_t first_index = 0;
				std::uint32_t index_count = 0;
			};

			GLuint vertex_buffer_ = 0;
			GLuint index_buffer_ = 0;
			GLsizei vertex_count_ = 0;
			GLsizei index_count_ = 0;
			std::vector<IndexedChunkMeshBatch> indexed_batches_;
		};
	}

	std::unique_ptr<RenderMesh> CreateOpenGLRenderMesh()
	{
		return std::make_unique<OpenGLRenderMesh>();
	}
}
