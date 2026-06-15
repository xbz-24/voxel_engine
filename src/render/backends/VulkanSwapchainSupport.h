#pragma once

#include "CoreTypes.h"

#include <volk.h>

namespace ve::rendering
{
	struct VulkanSwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities{};
		ve::core::DynamicArray<VkSurfaceFormatKHR> formats{};
		ve::core::DynamicArray<VkPresentModeKHR> present_modes{};

		/** @return True when format and present mode choices are available. */
		[[nodiscard]] bool IsComplete() const noexcept;
	};

	/** @param physical_device GPU to query. @param surface Presentation surface. @return Swapchain support details. */
	[[nodiscard]] VulkanSwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
}
