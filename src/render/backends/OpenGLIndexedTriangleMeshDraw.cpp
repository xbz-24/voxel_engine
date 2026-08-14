#include "OpenGLIndexedTriangleMesh.h"

#include <glm/gtc/type_ptr.hpp>

namespace ve::rendering
{
	void OpenGLIndexedTriangleMesh::Draw(const glm::mat4& model_matrix) const
	{
		if (vertex_array_ == 0 || index_count_ == 0) return;
		GLint previous_vertex_array = 0;
		GLint previous_matrix_mode = GL_MODELVIEW;
		GLint previous_active_texture = GL_TEXTURE0;
		GLint previous_program = 0;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previous_vertex_array);
		glGetIntegerv(GL_MATRIX_MODE, &previous_matrix_mode);
		glGetIntegerv(GL_ACTIVE_TEXTURE, &previous_active_texture);
		glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(glm::value_ptr(model_matrix));
		glPushAttrib(GL_CURRENT_BIT);
		glActiveTexture(GL_TEXTURE0);
		const GLboolean texture_was_enabled = glIsEnabled(GL_TEXTURE_2D);
		const GLboolean lighting_was_enabled = glIsEnabled(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glUseProgram(0);
		glBindVertexArray(vertex_array_);
		glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(static_cast<GLuint>(previous_vertex_array));
		if (texture_was_enabled == GL_TRUE) glEnable(GL_TEXTURE_2D);
		if (lighting_was_enabled == GL_TRUE) glEnable(GL_LIGHTING);
		glUseProgram(static_cast<GLuint>(previous_program));
		glActiveTexture(static_cast<GLenum>(previous_active_texture));
		glPopAttrib();
		glPopMatrix();
		glMatrixMode(static_cast<GLenum>(previous_matrix_mode));
	}
}
