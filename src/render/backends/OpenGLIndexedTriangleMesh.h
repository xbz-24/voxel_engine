#pragma once

#include "IndexedTriangleMesh.h"

#include <GL/glew.h>

namespace ve::rendering
{
	/** Compatibility-profile untextured vertex-color mesh stored in one VAO, VBO, and EBO. */
	class OpenGLIndexedTriangleMesh final : public IndexedTriangleMesh
	{
	public:
		OpenGLIndexedTriangleMesh() = default;
		~OpenGLIndexedTriangleMesh() override;
		OpenGLIndexedTriangleMesh(const OpenGLIndexedTriangleMesh&) = delete;
		OpenGLIndexedTriangleMesh& operator=(const OpenGLIndexedTriangleMesh&) = delete;

		[[nodiscard]] bool Upload(
			const IndexedTriangleMeshDescription& description) override;
		void Draw(const glm::mat4& model_matrix) const override;
		void Release() noexcept override;

	private:
		GLuint vertex_array_ = 0;
		GLuint vertex_buffer_ = 0;
		GLuint index_buffer_ = 0;
		GLsizei index_count_ = 0;
	};
}
