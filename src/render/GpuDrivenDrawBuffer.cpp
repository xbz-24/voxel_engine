#include "GpuDrivenDrawBuffer.h"

namespace ve::rendering
{
	/// Converts engine draw commands into Vulkan indirect draw commands.
	void GpuDrivenDrawBuffer::Upload(std::span<const GpuDrawElementsCommand> commands)
	{
		commands_.clear();
		commands_.reserve(commands.size());
		for (const GpuDrawElementsCommand& command : commands)
		{
			commands_.push_back({ command.index_count, command.instance_count, command.first_index, command.vertex_offset, command.first_instance });
		}
	}

	/// Clears command storage and forgets the device buffer.
	void GpuDrivenDrawBuffer::Release()
	{
		commands_.clear();
		device_buffer_ = VK_NULL_HANDLE;
	}

	/// Assigns an externally allocated Vulkan command buffer.
	void GpuDrivenDrawBuffer::SetDeviceBuffer(VkBuffer buffer) noexcept { device_buffer_ = buffer; }

	/// Returns the Vulkan command buffer handle.
	VkBuffer GpuDrivenDrawBuffer::DeviceBuffer() const noexcept { return device_buffer_; }

	/// Returns CPU-side indirect commands.
	std::span<const VkDrawIndexedIndirectCommand> GpuDrivenDrawBuffer::Commands() const noexcept { return commands_; }

	/// Returns number of commands available for GPU-driven submission.
	ve::core::Index GpuDrivenDrawBuffer::CommandCount() const noexcept { return commands_.size(); }
}
