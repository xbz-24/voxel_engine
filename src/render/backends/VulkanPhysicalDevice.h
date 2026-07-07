#pragma once

#include <volk.h>
#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <vector>

namespace ve::rendering
{
	struct VulkanPhysicalDeviceSettings
	{
		bool require_swapchain_support = true;
	};

	struct VulkanPhysicalDeviceSelectionCriteria
	{
		VulkanPhysicalDeviceSettings settings{};
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		std::vector<const char*> required_device_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};

	struct VulkanQueueFamilyIndices
	{
		std::uint32_t graphics_family = UINT32_MAX;
		std::uint32_t present_family = UINT32_MAX;

		/** @return True when graphics and present queues were found. */
		[[nodiscard]] bool IsComplete() const noexcept;
	};

	/** Selects and stores the Vulkan physical device used by the renderer. */
	class VulkanPhysicalDevice
	{
	public:
		/** @param instance Vulkan instance. @param surface Presentation surface. @return True when a suitable GPU is found. */
		[[nodiscard]] bool Select(VkInstance instance, VkSurfaceKHR surface);

		/** @param instance Vulkan instance. @param criteria Device requirements. @return True when a suitable GPU is found. */
		[[nodiscard]] bool Select(VkInstance instance, const VulkanPhysicalDeviceSelectionCriteria& criteria);

		/** @return Selected physical device, or VK_NULL_HANDLE when none was selected. */
		[[nodiscard]] VkPhysicalDevice Handle() const noexcept;

		/** @return C++ Vulkan-Hpp physical device handle. */
		[[nodiscard]] vk::PhysicalDevice CppHandle() const noexcept;

		/** @return Queue family indices for the selected device. */
		[[nodiscard]] const VulkanQueueFamilyIndices& QueueFamilies() const noexcept;

	private:
		VkPhysicalDevice device_ = VK_NULL_HANDLE;
		VulkanQueueFamilyIndices queue_families_{};
	};
}
