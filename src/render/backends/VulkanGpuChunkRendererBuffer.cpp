#include "VulkanGpuChunkRenderer.h"

#include <cstring>

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreateBuffer(VkDeviceSize byte_size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& memory) const
	{
		VkBufferCreateInfo buffer_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer = VK_NULL_HANDLE;
		memory = VK_NULL_HANDLE;
		buffer_info.size = byte_size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (vkCreateBuffer(device_, &buffer_info, nullptr, &buffer) != VK_SUCCESS) return false;

		VkMemoryRequirements requirements{};
		vkGetBufferMemoryRequirements(device_, buffer, &requirements);
		const std::uint32_t memory_type = FindMemoryType(physical_device_, requirements.memoryTypeBits, properties);
		if (memory_type == UINT32_MAX)
		{
			vkDestroyBuffer(device_, buffer, nullptr);
			buffer = VK_NULL_HANDLE;
			return false;
		}

		VkMemoryAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocate_info.allocationSize = requirements.size;
		allocate_info.memoryTypeIndex = memory_type;
		if (vkAllocateMemory(device_, &allocate_info, nullptr, &memory) != VK_SUCCESS)
		{
			vkDestroyBuffer(device_, buffer, nullptr);
			buffer = VK_NULL_HANDLE;
			return false;
		}
		if (vkBindBufferMemory(device_, buffer, memory, 0) != VK_SUCCESS)
		{
			vkFreeMemory(device_, memory, nullptr);
			vkDestroyBuffer(device_, buffer, nullptr);
			memory = VK_NULL_HANDLE;
			buffer = VK_NULL_HANDLE;
			return false;
		}
		return true;
	}
	bool VulkanGpuChunkRenderer::CreateHostBuffer(VkDeviceSize byte_size, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory) const
	{
		return CreateBuffer(byte_size,
			usage,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			buffer,
			memory);
	}
	bool VulkanGpuChunkRenderer::CopyToDeviceBuffer(VkDeviceMemory memory, const void* source, VkDeviceSize byte_size) const
	{
		void* mapped = nullptr;
		if (vkMapMemory(device_, memory, 0, byte_size, 0, &mapped) != VK_SUCCESS) return false;
		std::memcpy(mapped, source, static_cast<std::size_t>(byte_size));
		vkUnmapMemory(device_, memory);
		return true;
	}
}
