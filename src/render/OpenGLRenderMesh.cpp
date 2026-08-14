#include "OpenGLRenderMesh.h"

#include <memory>

namespace ve::rendering
{
	OpenGLRenderMesh::~OpenGLRenderMesh() noexcept
	{
		Release();
	}

	void OpenGLRenderMesh::Release() noexcept
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

	std::unique_ptr<RenderMesh> CreateOpenGLRenderMesh()
	{
		return std::make_unique<OpenGLRenderMesh>();
	}
}
