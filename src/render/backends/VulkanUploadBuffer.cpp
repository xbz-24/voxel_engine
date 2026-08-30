#include "VulkanUploadBuffer.h"

#include "VulkanBackend.h"

namespace ve::rendering
{
	std::uint32_t VulkanUploadBuffer::FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memory_properties{};
		vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
		for (std::uint32_t index = 0; index < memory_properties.memoryTypeCount; ++index)
		{
			const bool is_allowed = (type_filter & (1u << index)) != 0;
			const bool has_properties = (memory_properties.memoryTypes[index].propertyFlags & properties) == properties;
			if (is_allowed && has_properties) return index;
		}
		return UINT32_MAX;
	}

	bool VulkanUploadBuffer::Create(VulkanBackend& backend, VkDeviceSize byte_size)
	{
		const VkDevice device = backend.Device().Handle();

		VkBufferCreateInfo buffer_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_info.size = byte_size;
		buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (vkCreateBuffer(device, &buffer_info, nullptr, &buffer_) != VK_SUCCESS) return false;

		VkMemoryRequirements requirements{};
		vkGetBufferMemoryRequirements(device, buffer_, &requirements);
		const std::uint32_t memory_type = FindMemoryType(backend.PhysicalDevice().Handle(), requirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (memory_type == UINT32_MAX)
		{
			Release(device);
			return false;
		}

		VkMemoryAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocate_info.allocationSize = requirements.size;
		allocate_info.memoryTypeIndex = memory_type;
		if (vkAllocateMemory(device, &allocate_info, nullptr, &memory_) != VK_SUCCESS)
		{
			Release(device);
			return false;
		}
		if (vkBindBufferMemory(device, buffer_, memory_, 0) != VK_SUCCESS)
		{
			Release(device);
			return false;
		}
		if (vkMapMemory(device, memory_, 0, byte_size, 0, &mapped_memory_) != VK_SUCCESS)
		{
			Release(device);
			return false;
		}
		size_ = byte_size;
		return true;
	}
}
