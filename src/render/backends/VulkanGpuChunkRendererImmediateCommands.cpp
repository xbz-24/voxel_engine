#include "VulkanGpuChunkRenderer.h"

#include "VulkanBackend.h"

namespace ve::rendering
{
	bool VulkanGpuChunkRendererResourceOperations::RunImmediateCommands(
		void (*record)(VkCommandBuffer, void*),
		void* user_data) const
	{
		VkCommandBuffer command_buffer = VK_NULL_HANDLE;
		VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocate_info.commandPool = command_pool_;
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandBufferCount = 1u;
		if (vkAllocateCommandBuffers(device_, &allocate_info, &command_buffer) != VK_SUCCESS)
		{
			return false;
		}
		const auto free_command_buffer = [&]
		{
			if (command_buffer != VK_NULL_HANDLE)
			{
				vkFreeCommandBuffers(device_, command_pool_, 1u, &command_buffer);
			}
		};

		VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
		{
			free_command_buffer();
			return false;
		}
		record(command_buffer, user_data);
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
		{
			free_command_buffer();
			return false;
		}

		VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit_info.commandBufferCount = 1u;
		submit_info.pCommandBuffers = &command_buffer;
		const VkQueue graphics_queue = backend_->Device().GraphicsQueue();
		const bool submitted = vkQueueSubmit(
			graphics_queue, 1u, &submit_info, VK_NULL_HANDLE) == VK_SUCCESS;
		if (submitted) vkQueueWaitIdle(graphics_queue);
		free_command_buffer();
		return submitted;
	}
}
