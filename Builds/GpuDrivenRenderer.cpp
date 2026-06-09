#include "GpuDrivenRenderer.h"

namespace ve::rendering
{
	/// Uploads commands to the GPU indirect draw buffer.
	void GpuDrivenRenderer::UploadCommands(std::span<const GpuDrawElementsCommand> commands)
	{
		draw_buffer_.Upload(commands);
	}

	/// Submits commands through multi-draw indirect when available.
	void GpuDrivenRenderer::DrawIndexedIndirect(GLenum index_type) const
	{
		if (draw_buffer_.CommandCount() == 0) return;
		draw_buffer_.Bind();
		glMultiDrawElementsIndirect(GL_TRIANGLES, index_type, nullptr, static_cast<GLsizei>(draw_buffer_.CommandCount()), 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	}

	/// Returns uploaded command count.
	ve::core::Index GpuDrivenRenderer::CommandCount() const noexcept
	{
		return draw_buffer_.CommandCount();
	}
}
