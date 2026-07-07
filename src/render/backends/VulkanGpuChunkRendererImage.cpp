#include "VulkanGpuChunkRenderer.h"

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreateDeviceImage(VkImageCreateInfo image_info, VkDeviceMemory& memory, VkImage& image) const
	{
		memory = VK_NULL_HANDLE;
		image = VK_NULL_HANDLE;
		if (vkCreateImage(device_, &image_info, nullptr, &image) != VK_SUCCESS) return false;
		VkMemoryRequirements requirements{};
		vkGetImageMemoryRequirements(device_, image, &requirements);
		const std::uint32_t memory_type = FindMemoryType(physical_device_, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (memory_type == UINT32_MAX)
		{
			vkDestroyImage(device_, image, nullptr);
			image = VK_NULL_HANDLE;
			return false;
		}
		VkMemoryAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocate_info.allocationSize = requirements.size;
		allocate_info.memoryTypeIndex = memory_type;
		if (vkAllocateMemory(device_, &allocate_info, nullptr, &memory) != VK_SUCCESS)
		{
			vkDestroyImage(device_, image, nullptr);
			image = VK_NULL_HANDLE;
			return false;
		}
		if (vkBindImageMemory(device_, image, memory, 0) != VK_SUCCESS)
		{
			vkFreeMemory(device_, memory, nullptr);
			vkDestroyImage(device_, image, nullptr);
			memory = VK_NULL_HANDLE;
			image = VK_NULL_HANDLE;
			return false;
		}
		return true;
	}
}
