#pragma once

#include "VulkanPhysicalDevice.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace ve::rendering
{
	struct VulkanDeviceSettings
	{
		float queue_priority = 1.0f;
		std::vector<const char*> required_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		std::vector<const char*> optional_extensions{ "VK_KHR_portability_subset" };
	};

	/** Owns the logical Vulkan device and renderer queues. */
	class VulkanDevice
	{
	public:
		/** Releases the logical device when destroyed. */
		~VulkanDevice();

		/** @param physical_device Selected GPU. @param queues Queue family indices. @return True when logical device creation succeeds. */
		[[nodiscard]] bool Create(VkPhysicalDevice physical_device, const VulkanQueueFamilyIndices& queues);

		/** @param physical_device Selected GPU. @param queues Queue family indices. @param settings Device creation settings. @return True when logical device creation succeeds. */
		[[nodiscard]] bool Create(
			VkPhysicalDevice physical_device,
			const VulkanQueueFamilyIndices& queues,
			const VulkanDeviceSettings& settings);

		/** Destroys the logical device and clears queues. */
		void Release();

		/** @return Logical Vulkan device, or VK_NULL_HANDLE. */
		[[nodiscard]] VkDevice Handle() const noexcept;

		/** @return C++ Vulkan-Hpp logical device handle. */
		[[nodiscard]] vk::Device CppHandle() const noexcept;

		/** @return Graphics command queue. */
		[[nodiscard]] VkQueue GraphicsQueue() const noexcept;

		/** @return Presentation queue. */
		[[nodiscard]] VkQueue PresentQueue() const noexcept;

	private:
		VkDevice device_ = VK_NULL_HANDLE;
		VkQueue graphics_queue_ = VK_NULL_HANDLE;
		VkQueue present_queue_ = VK_NULL_HANDLE;
	};
}
