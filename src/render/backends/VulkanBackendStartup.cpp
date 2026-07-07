#include "VulkanBackendStartup.h"

#include "Window.h"

#include <algorithm>
#include <cstring>
#include <span>

namespace ve::rendering
{
	namespace
	{
		void AppendUniqueExtensions(
			std::vector<const char*>& target_extensions,
			std::span<const char* const> source_extensions)
		{
			for (const char* extension_name : source_extensions)
			{
				const auto existing = std::ranges::find_if(target_extensions, [extension_name](const char* target_extension)
				{
					return std::strcmp(target_extension, extension_name) == 0;
				});
				if (existing == target_extensions.end()) target_extensions.push_back(extension_name);
			}
		}
	}

	VulkanBackendWindowSettings CaptureVulkanWindowSettings(const ve::engine::Window& window)
	{
		return VulkanBackendWindowSettings{
			window.RequiredVulkanInstanceExtensions(),
			std::max(window.GetWidth(), 1),
			std::max(window.GetHeight(), 1),
			window.IsVSyncEnabled()
		};
	}

	VulkanContextSettings BuildVulkanContextSettings(
		const VulkanBackendSettings& settings,
		const VulkanBackendWindowSettings& window_settings)
	{
		VulkanContextSettings context_settings = settings.context;
		AppendUniqueExtensions(context_settings.required_extensions, window_settings.required_instance_extensions);
		return context_settings;
	}

	VulkanPhysicalDeviceSelectionCriteria BuildVulkanPhysicalDeviceCriteria(
		const VulkanBackendSettings& settings,
		VkSurfaceKHR surface)
	{
		return VulkanPhysicalDeviceSelectionCriteria{
			settings.physical_device,
			surface,
			settings.device.required_extensions
		};
	}

	VulkanSwapchainSettings BuildVulkanSwapchainSettings(
		const VulkanBackendSettings& settings,
		const VulkanBackendWindowSettings& window_settings)
	{
		VulkanSwapchainSettings swapchain_settings = settings.swapchain;
		if (swapchain_settings.width <= 0) swapchain_settings.width = window_settings.swapchain_width;
		if (swapchain_settings.height <= 0) swapchain_settings.height = window_settings.swapchain_height;
		swapchain_settings.enable_vsync = window_settings.enable_vsync;
		return swapchain_settings;
	}
}
