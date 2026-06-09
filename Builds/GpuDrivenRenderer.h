#pragma once

#include "GpuDrivenDrawBuffer.h"

namespace ve::rendering
{
	/** Submits indexed geometry through OpenGL multi-draw indirect calls. */
	class GpuDrivenRenderer
	{
	public:
		/** @param commands Commands copied to the indirect draw buffer. */
		void UploadCommands(std::span<const GpuDrawElementsCommand> commands);

		/** @param index_type OpenGL index type, usually GL_UNSIGNED_INT. */
		void DrawIndexedIndirect(GLenum index_type = GL_UNSIGNED_INT) const;

		/** @return Uploaded command count. */
		[[nodiscard]] ve::core::Index CommandCount() const noexcept;

	private:
		GpuDrivenDrawBuffer draw_buffer_;
	};
}
