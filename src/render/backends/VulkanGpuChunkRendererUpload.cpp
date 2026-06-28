#include "VulkanGpuChunkRenderer.h"

#include "VulkanBackend.h"

namespace ve::rendering
{
	namespace
	{
		struct BufferCopyContext
		{
			VkBuffer source = VK_NULL_HANDLE;
			VkBuffer destination = VK_NULL_HANDLE;
			VkDeviceSize byte_size = 0;
		};

		void RecordBufferCopy(VkCommandBuffer command_buffer, void* user_data)
		{
			const auto& context = *static_cast<const BufferCopyContext*>(user_data);
			VkBufferCopy copy{};
			copy.size = context.byte_size;
			vkCmdCopyBuffer(command_buffer, context.source, context.destination, 1u, &copy);

			VkBufferMemoryBarrier to_vertex_input{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
			to_vertex_input.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			to_vertex_input.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
			to_vertex_input.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			to_vertex_input.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			to_vertex_input.buffer = context.destination;
			to_vertex_input.size = context.byte_size;
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
				0, 0, nullptr, 1, &to_vertex_input, 0, nullptr);
		}
	}
	bool VulkanGpuChunkRenderer::UploadDeviceLocalBuffer(const void* source,
		VkDeviceSize byte_size,
		VkBufferUsageFlags usage,
		VkBuffer& buffer,
		VkDeviceMemory& memory) const
	{
		VkBuffer staging = VK_NULL_HANDLE;
		VkDeviceMemory staging_memory = VK_NULL_HANDLE;
		if (!CreateHostBuffer(byte_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging, staging_memory) ||
			!CopyToDeviceBuffer(staging_memory, source, byte_size) ||
			!CreateBuffer(byte_size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory))
		{
			if (staging != VK_NULL_HANDLE) vkDestroyBuffer(device_, staging, nullptr);
			if (staging_memory != VK_NULL_HANDLE) vkFreeMemory(device_, staging_memory, nullptr);
			return false;
		}

		BufferCopyContext context{ staging, buffer, byte_size };
		const bool copied = RunImmediateCommands(RecordBufferCopy, &context);
		vkDestroyBuffer(device_, staging, nullptr);
		vkFreeMemory(device_, staging_memory, nullptr);
		return copied;
	}
	bool VulkanGpuChunkRenderer::RunImmediateCommands(void (*record)(VkCommandBuffer, void*), void* user_data) const
	{
		VkCommandBuffer command_buffer = VK_NULL_HANDLE;
		VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocate_info.commandPool = command_pool_;
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandBufferCount = 1u;
		if (vkAllocateCommandBuffers(device_, &allocate_info, &command_buffer) != VK_SUCCESS) return false;
		const auto free_command_buffer = [&]
		{
			if (command_buffer != VK_NULL_HANDLE) vkFreeCommandBuffers(device_, command_pool_, 1u, &command_buffer);
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
		const bool submitted = vkQueueSubmit(backend_->Device().GraphicsQueue(), 1u, &submit_info, VK_NULL_HANDLE) == VK_SUCCESS;
		if (submitted) vkQueueWaitIdle(backend_->Device().GraphicsQueue());
		free_command_buffer();
		return submitted;
	}
}
