#include "OpenGLIndexedTriangleMesh.h"

namespace ve::rendering
{
	OpenGLIndexedTriangleMesh::~OpenGLIndexedTriangleMesh()
	{
		Release();
	}

	void OpenGLIndexedTriangleMesh::Release() noexcept
	{
		if (vertex_array_ != 0) glDeleteVertexArrays(1, &vertex_array_);
		if (vertex_buffer_ != 0) glDeleteBuffers(1, &vertex_buffer_);
		if (index_buffer_ != 0) glDeleteBuffers(1, &index_buffer_);
		vertex_array_ = 0;
		vertex_buffer_ = 0;
		index_buffer_ = 0;
		index_count_ = 0;
	}
}
