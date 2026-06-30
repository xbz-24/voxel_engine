#pragma once

#include "VulkanBackendSettings.h"

#include <vector>

namespace ve::engine { class Window; }

namespace ve::rendering
{
	struct VulkanBackendWindowSettings
	{
		std::vector<const char*> required_instance_extensions{};
		int swapchain_width = 1;
		int swapchain_height = 1;
		bool enable_vsync = false;
	};

	[[nodiscard]] VulkanBackendWindowSettings CaptureVulkanWindowSettings(const ve::engine::Window& window);

	[[nodiscard]] VulkanContextSettings BuildVulkanContextSettings(
		const VulkanBackendSettings& settings,
		const VulkanBackendWindowSettings& window_settings);

	[[nodiscard]] VulkanPhysicalDeviceSelectionCriteria BuildVulkanPhysicalDeviceCriteria(
		const VulkanBackendSettings& settings,
		VkSurfaceKHR surface);

	[[nodiscard]] VulkanSwapchainSettings BuildVulkanSwapchainSettings(
		const VulkanBackendSettings& settings,
		const VulkanBackendWindowSettings& window_settings);
}
