#include "OpenGLIndexedTriangleMesh.h"

#include "OpenGLTypeConversions.h"

#include <cstddef>
#include <limits>

namespace ve::rendering
{
	namespace
	{
		struct PreviousUploadState
		{
			GLint vertex_array = 0;
			GLint array_buffer = 0;
			GLint element_buffer = 0;
			GLint client_texture = GL_TEXTURE0;
		};

		[[nodiscard]] PreviousUploadState CaptureUploadState()
		{
			PreviousUploadState state;
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &state.vertex_array);
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &state.array_buffer);
			glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &state.element_buffer);
			glGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE, &state.client_texture);
			return state;
		}

		void RestoreUploadState(const PreviousUploadState& state)
		{
			glBindVertexArray(static_cast<GLuint>(state.vertex_array));
			glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(state.array_buffer));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(state.element_buffer));
			glClientActiveTexture(static_cast<GLenum>(state.client_texture));
		}

		[[nodiscard]] bool FitsOpenGL(const IndexedTriangleMeshDescription& description)
		{
			const auto max_bytes = static_cast<std::size_t>(
				std::numeric_limits<GLsizeiptr>::max());
			return description.indices.size() <= static_cast<std::size_t>(
				std::numeric_limits<GLsizei>::max()) &&
				description.vertices.size_bytes() <= max_bytes &&
				description.indices.size_bytes() <= max_bytes;
		}
	}

	bool OpenGLIndexedTriangleMesh::Upload(
		const IndexedTriangleMeshDescription& description)
	{
		Release();
		if (!description.IsValid() || !FitsOpenGL(description)) return false;
		const PreviousUploadState previous = CaptureUploadState();
		glGenVertexArrays(1, &vertex_array_);
		glGenBuffers(1, &vertex_buffer_);
		glGenBuffers(1, &index_buffer_);
		if (vertex_array_ == 0 || vertex_buffer_ == 0 || index_buffer_ == 0)
		{
			RestoreUploadState(previous);
			Release();
			return false;
		}

		glBindVertexArray(vertex_array_);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
		glBufferData(GL_ARRAY_BUFFER, OpenGLByteCount(description.vertices.size_bytes()),
			description.vertices.data(), GL_STATIC_DRAW);
		GLint64 uploaded_vertex_bytes = 0;
		glGetBufferParameteri64v(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &uploaded_vertex_bytes);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, OpenGLByteCount(description.indices.size_bytes()),
			description.indices.data(), GL_STATIC_DRAW);
		GLint64 uploaded_index_bytes = 0;
		glGetBufferParameteri64v(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &uploaded_index_bytes);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glClientActiveTexture(GL_TEXTURE0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D),
			reinterpret_cast<void*>(offsetof(Vertex3D, position)));
		glNormalPointer(GL_FLOAT, sizeof(Vertex3D),
			reinterpret_cast<void*>(offsetof(Vertex3D, normal)));
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D),
			reinterpret_cast<void*>(offsetof(Vertex3D, uv)));
		glColorPointer(4, GL_FLOAT, sizeof(Vertex3D),
			reinterpret_cast<void*>(offsetof(Vertex3D, color)));
		index_count_ = OpenGLCount(description.indices.size());
		RestoreUploadState(previous);
		const bool uploaded = uploaded_vertex_bytes ==
			OpenGLByteCount(description.vertices.size_bytes()) && uploaded_index_bytes ==
			OpenGLByteCount(description.indices.size_bytes());
		if (!uploaded) Release();
		return uploaded;
	}
}
