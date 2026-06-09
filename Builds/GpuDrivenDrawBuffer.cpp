#include "GpuDrivenDrawBuffer.h"

namespace ve::rendering
{
	/// Releases the OpenGL indirect buffer.
	GpuDrivenDrawBuffer::~GpuDrivenDrawBuffer() { Release(); }

	/// Uploads draw commands into GL_DRAW_INDIRECT_BUFFER.
	void GpuDrivenDrawBuffer::Upload(std::span<const GpuDrawElementsCommand> commands)
	{
		if (buffer_ == 0) glGenBuffers(1, &buffer_);
		command_count_ = commands.size();
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer_);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size_bytes(), commands.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	}

	/// Binds the indirect draw command buffer.
	void GpuDrivenDrawBuffer::Bind() const
	{
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer_);
	}

	/// Releases the OpenGL indirect buffer.
	void GpuDrivenDrawBuffer::Release()
	{
		if (buffer_ != 0) glDeleteBuffers(1, &buffer_);
		buffer_ = 0;
		command_count_ = 0;
	}

	/// Returns number of commands available for GPU-driven submission.
	ve::core::Index GpuDrivenDrawBuffer::CommandCount() const noexcept { return command_count_; }
}
