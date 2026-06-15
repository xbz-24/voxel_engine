#include "VulkanSwapchainSupport.h"

namespace ve::rendering
{
	/** Reports whether swapchain support has enough options to create a chain. */
	bool VulkanSwapchainSupportDetails::IsComplete() const noexcept
	{
		return !formats.empty() && !present_modes.empty();
	}

	/** Queries surface capabilities, formats and present modes. */
	VulkanSwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
	{
		VulkanSwapchainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details.capabilities);
		std::uint32_t format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
		details.formats.resize(format_count);
		if (format_count > 0) vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details.formats.data());
		std::uint32_t present_mode_count = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
		details.present_modes.resize(present_mode_count);
		if (present_mode_count > 0) vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, details.present_modes.data());
		return details;
	}
}
