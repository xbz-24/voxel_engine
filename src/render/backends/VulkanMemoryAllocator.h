#pragma once

#include <volk.h>

typedef struct VmaAllocator_T* VmaAllocator;

namespace ve::rendering
{
	/** Wraps GPUOpen Vulkan Memory Allocator for engine-owned GPU memory. */
	class VulkanMemoryAllocator
	{
	public:
		/** Destroys the allocator when the wrapper dies. */
		~VulkanMemoryAllocator();

		/** @param instance Vulkan instance. @param physical_device Selected GPU. @param device Logical device. @return True when VMA is ready. */
		[[nodiscard]] bool Initialize(VkInstance instance, VkPhysicalDevice physical_device, VkDevice device);

		/** Destroys the VMA allocator and resets the handle. */
		void Release();

		/** @return Raw VMA allocator handle, or null when inactive. */
		[[nodiscard]] VmaAllocator Handle() const noexcept;

		/** @return True when the allocator was created successfully. */
		[[nodiscard]] bool IsInitialized() const noexcept;

	private:
		VmaAllocator allocator_ = nullptr;
	};
}
