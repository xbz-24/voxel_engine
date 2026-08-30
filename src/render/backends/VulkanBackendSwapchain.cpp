#include "VulkanBackend.h"

namespace ve::rendering
{
	bool VulkanBackend::RecreateSwapchain(int width, int height, bool enable_vsync)
	{
		if (width <= 0 || height <= 0 ||
			physical_device_.Handle() == VK_NULL_HANDLE ||
			device_.Handle() == VK_NULL_HANDLE ||
			surface_.Handle() == VK_NULL_HANDLE)
		{
			return false;
		}

		VulkanSwapchainSettings next_settings = swapchain_settings_;
		next_settings.width = width;
		next_settings.height = height;
		next_settings.enable_vsync = enable_vsync;
		if (!swapchain_.Create(
			physical_device_.Handle(),
			device_.Handle(),
			surface_.Handle(),
			next_settings))
		{
			return false;
		}
		swapchain_settings_ = next_settings;
		return true;
	}
}
