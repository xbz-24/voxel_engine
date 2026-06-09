#pragma once

#include "CoreTypes.h"

#include <GL/glew.h>
#include <span>

namespace ve::rendering
{
	/** OpenGL-compatible indirect draw command for glMultiDrawElementsIndirect. */
	struct GpuDrawElementsCommand
	{
		GLuint index_count = 0;
		GLuint instance_count = 1;
		GLuint first_index = 0;
		GLuint base_vertex = 0;
		GLuint base_instance = 0;
	};

	/** Owns the GPU buffer containing indirect draw commands. */
	class GpuDrivenDrawBuffer
	{
	public:
		/** Releases the OpenGL indirect buffer. */
		~GpuDrivenDrawBuffer();

		/** @param commands Draw commands uploaded to GL_DRAW_INDIRECT_BUFFER. */
		void Upload(std::span<const GpuDrawElementsCommand> commands);

		/** Binds the indirect draw command buffer. */
		void Bind() const;

		/** Releases the OpenGL indirect buffer. */
		void Release();

		/** @return Number of commands available for GPU-driven submission. */
		[[nodiscard]] ve::core::Index CommandCount() const noexcept;

	private:
		GLuint buffer_ = 0;
		ve::core::Index command_count_ = 0;
	};
}
