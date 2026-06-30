#pragma once

#include "VulkanDevice.h"

#include <span>
#include <vector>

namespace ve::rendering
{
	[[nodiscard]] bool VulkanDeviceSupportsExtension(VkPhysicalDevice physical_device, const char* extension_name);

	[[nodiscard]] bool VulkanDeviceSupportsExtensions(
		VkPhysicalDevice physical_device,
		std::span<const char* const> required_extensions);

	[[nodiscard]] bool VulkanDeviceSupportsRequiredExtensions(
		VkPhysicalDevice physical_device,
		const VulkanDeviceSettings& settings);

	[[nodiscard]] std::vector<const char*> SelectEnabledVulkanDeviceExtensions(
		VkPhysicalDevice physical_device,
		const VulkanDeviceSettings& settings);
}
