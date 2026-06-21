#include "RenderMesh.h"

#include "TextureLoader.h"

#include <GL/glew.h>

#include <cstddef>
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

				glGenBuffers(1, &vertex_buffer_);
				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
				glBufferData(GL_ARRAY_BUFFER,
					static_cast<GLsizeiptr>(description.vertices.size_bytes()),
					description.vertices.data(),
					GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				vertex_count_ = static_cast<GLsizei>(description.vertices.size());
				batches_.assign(description.batches.begin(), description.batches.end());
			}

			void Draw() const override
			{
				if (vertex_buffer_ == 0 || vertex_count_ == 0) return;

				// TODO: Migrate compatibility meshes to indexed triangles so OpenGL and Vulkan consume the same mesh topology.
				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);
				glVertexPointer(3, GL_FLOAT, sizeof(ChunkVertex), reinterpret_cast<void*>(offsetof(ChunkVertex, x)));
				glTexCoordPointer(2, GL_FLOAT, sizeof(ChunkVertex), reinterpret_cast<void*>(offsetof(ChunkVertex, u)));
				glColorPointer(3, GL_FLOAT, sizeof(ChunkVertex), reinterpret_cast<void*>(offsetof(ChunkVertex, r)));

				for (const ChunkMeshBatch& batch : batches_)
				{
					glBindTexture(GL_TEXTURE_2D, NativeOpenGLTexture(batch.texture));
					glDrawArrays(GL_QUADS, static_cast<GLint>(batch.first_vertex), static_cast<GLsizei>(batch.vertex_count));
				}

				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
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
				vertex_count_ = 0;
				batches_.clear();
			}

		private:
			GLuint vertex_buffer_ = 0;
			GLsizei vertex_count_ = 0;
			std::vector<ChunkMeshBatch> batches_;
		};
	}

	std::unique_ptr<RenderMesh> CreateOpenGLRenderMesh()
	{
		return std::make_unique<OpenGLRenderMesh>();
	}
}
