#pragma once

#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSwapchain.h"

namespace ve::rendering
{
	/** Startup knobs used by VulkanBackend before concrete Vulkan objects exist. */
	struct VulkanBackendSettings
	{
		VulkanContextSettings context{};
		VulkanPhysicalDeviceSettings physical_device{};
		VulkanDeviceSettings device{};
		VulkanSwapchainSettings swapchain{};
	};
}
