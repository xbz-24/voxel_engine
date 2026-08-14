#pragma once

#include "RenderMesh.h"

#include <GL/glew.h>

#include <cstdint>
#include <vector>

namespace ve::rendering
{
	/** OpenGL vertex/index buffers backing one chunk mesh. */
	class OpenGLRenderMesh final : public RenderMesh
	{
	public:
		OpenGLRenderMesh() = default;
		~OpenGLRenderMesh() noexcept override;

		OpenGLRenderMesh(const OpenGLRenderMesh&) = delete;
		OpenGLRenderMesh& operator=(const OpenGLRenderMesh&) = delete;

		void Upload(const MeshDescription& description) override;
		void Draw() const override;
		void Release() noexcept override;

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
