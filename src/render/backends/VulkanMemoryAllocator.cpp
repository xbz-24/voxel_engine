#include "VulkanMemoryAllocator.h"

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace ve::rendering
{
	/** Releases the VMA allocator. */
	VulkanMemoryAllocator::~VulkanMemoryAllocator() { Release(); }

	/** Initializes VMA with externally created Vulkan handles. */
	bool VulkanMemoryAllocator::Initialize(VkInstance instance, VkPhysicalDevice physical_device, VkDevice device)
	{
		Release();
		VmaAllocatorCreateInfo create_info{};
		create_info.instance = instance;
		create_info.physicalDevice = physical_device;
		create_info.device = device;
		create_info.vulkanApiVersion = VK_API_VERSION_1_3;
		VmaVulkanFunctions vulkan_functions{};
		if (vmaImportVulkanFunctionsFromVolk(&create_info, &vulkan_functions) != VK_SUCCESS) return false;
		create_info.pVulkanFunctions = &vulkan_functions;
		return vmaCreateAllocator(&create_info, &allocator_) == VK_SUCCESS;
	}

	/** Destroys the VMA allocator. */
	void VulkanMemoryAllocator::Release()
	{
		if (allocator_) vmaDestroyAllocator(allocator_);
		allocator_ = nullptr;
	}

	/** Returns the raw VMA allocator handle. */
	VmaAllocator VulkanMemoryAllocator::Handle() const noexcept { return allocator_; }

	/** Reports whether the VMA allocator is active. */
	bool VulkanMemoryAllocator::IsInitialized() const noexcept { return allocator_ != nullptr; }
}
