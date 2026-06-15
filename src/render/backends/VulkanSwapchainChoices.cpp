#include "VulkanSwapchainChoices.h"

#include <algorithm>

namespace ve::rendering
{
	/** Chooses SRGB BGRA when available, otherwise the first reported format. */
	VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(std::span<const VkSurfaceFormatKHR> formats) noexcept
	{
		for (const VkSurfaceFormatKHR& format : formats)
		{
			const bool is_bgra = format.format == VK_FORMAT_B8G8R8A8_SRGB;
			const bool is_srgb = format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			if (is_bgra && is_srgb) return format;
		}
		return formats.empty() ? VkSurfaceFormatKHR{} : formats.front();
	}

	/** Chooses mailbox when available, otherwise FIFO which Vulkan guarantees. */
	VkPresentModeKHR ChooseSwapchainPresentMode(std::span<const VkPresentModeKHR> present_modes) noexcept
	{
		for (VkPresentModeKHR mode : present_modes)
		{
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) return mode;
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	/** Chooses a swapchain extent clamped to surface capabilities. */
	VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height) noexcept
	{
		if (capabilities.currentExtent.width != UINT32_MAX) return capabilities.currentExtent;
		VkExtent2D extent{ static_cast<std::uint32_t>(std::max(width, 1)), static_cast<std::uint32_t>(std::max(height, 1)) };
		extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return extent;
	}
}
