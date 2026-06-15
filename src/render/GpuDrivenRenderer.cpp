#include "GpuDrivenRenderer.h"

namespace ve::rendering
{
	/// Uploads commands to the GPU indirect draw buffer.
	void GpuDrivenRenderer::UploadCommands(std::span<const GpuDrawElementsCommand> commands)
	{
		draw_buffer_.Upload(commands);
	}

	/// Records Vulkan indexed draw commands into the provided command buffer.
	void GpuDrivenRenderer::RecordIndexedIndirect(VkCommandBuffer command_buffer) const
	{
		if (draw_buffer_.CommandCount() == 0) return;
		if (draw_buffer_.DeviceBuffer() != VK_NULL_HANDLE)
		{
			vkCmdDrawIndexedIndirect(command_buffer, draw_buffer_.DeviceBuffer(), 0,
				static_cast<std::uint32_t>(draw_buffer_.CommandCount()), sizeof(VkDrawIndexedIndirectCommand));
			return;
		}
		for (const VkDrawIndexedIndirectCommand& command : draw_buffer_.Commands())
		{
			vkCmdDrawIndexed(command_buffer, command.indexCount, command.instanceCount, command.firstIndex, command.vertexOffset, command.firstInstance);
		}
	}

	/// Returns uploaded command count.
	ve::core::Index GpuDrivenRenderer::CommandCount() const noexcept
	{
		return draw_buffer_.CommandCount();
	}
}
