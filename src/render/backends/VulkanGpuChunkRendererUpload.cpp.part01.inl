#include "VulkanGpuChunkRenderer.h"

#include "VulkanBackend.h"

namespace ve::rendering
{
	namespace
	{
		void ReleaseBuffer(VkDevice device, VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize& capacity_bytes)
		{
			if (buffer != VK_NULL_HANDLE) vkDestroyBuffer(device, buffer, nullptr);
			if (memory != VK_NULL_HANDLE) vkFreeMemory(device, memory, nullptr);
			buffer = VK_NULL_HANDLE;
			memory = VK_NULL_HANDLE;
			capacity_bytes = 0;
		}

		struct BufferCopyContext
		{
			VkBuffer source = VK_NULL_HANDLE;
			VkBuffer destination = VK_NULL_HANDLE;
			VkDeviceSize byte_size = 0;
		};

		[[nodiscard]] const BufferCopyContext& BufferCopyFromUserData(void* user_data) noexcept
		{
			return *static_cast<const BufferCopyContext*>(user_data);
		}

		void RecordBufferCopy(VkCommandBuffer command_buffer, void* user_data)
		{
			const BufferCopyContext& context = BufferCopyFromUserData(user_data);
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
		VkDeviceMemory& memory,
		VkDeviceSize& capacity_bytes) const
	{
		VkBuffer staging = VK_NULL_HANDLE;
		VkDeviceMemory staging_memory = VK_NULL_HANDLE;
		if (!CreateHostBuffer(byte_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging, staging_memory) ||
			!CopyToDeviceBuffer(staging_memory, source, byte_size))
		{
			if (staging != VK_NULL_HANDLE) vkDestroyBuffer(device_, staging, nullptr);
			if (staging_memory != VK_NULL_HANDLE) vkFreeMemory(device_, staging_memory, nullptr);
			return false;
		}

		if (buffer == VK_NULL_HANDLE || capacity_bytes < byte_size)
		{
			ReleaseBuffer(device_, buffer, memory, capacity_bytes);
			const VkBufferUsageFlags device_usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			if (!CreateBuffer(byte_size, device_usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory))
			{
				vkDestroyBuffer(device_, staging, nullptr);
				vkFreeMemory(device_, staging_memory, nullptr);
				return false;
			}
			capacity_bytes = byte_size;
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
