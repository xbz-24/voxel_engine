#include "VulkanDeviceExtensions.h"

#include <algorithm>
#include <cstring>

namespace ve::rendering
{
	bool VulkanDeviceSupportsExtension(VkPhysicalDevice physical_device, const char* extension_name)
	{
		std::uint32_t extension_count = 0;
		if (vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr) != VK_SUCCESS) return false;
		std::vector<VkExtensionProperties> extensions(extension_count);
		if (extension_count > 0 && vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extensions.data()) != VK_SUCCESS) return false;
		return std::ranges::any_of(extensions, [extension_name](const VkExtensionProperties& extension)
		{
			return std::strcmp(extension.extensionName, extension_name) == 0;
		});
	}

	bool VulkanDeviceSupportsExtensions(
		VkPhysicalDevice physical_device,
		std::span<const char* const> required_extensions)
	{
		return std::ranges::all_of(required_extensions, [physical_device](const char* extension_name)
		{
			return VulkanDeviceSupportsExtension(physical_device, extension_name);
		});
	}

	bool VulkanDeviceSupportsRequiredExtensions(
		VkPhysicalDevice physical_device,
		const VulkanDeviceSettings& settings)
	{
		return VulkanDeviceSupportsExtensions(physical_device, settings.required_extensions);
	}

	std::vector<const char*> SelectEnabledVulkanDeviceExtensions(
		VkPhysicalDevice physical_device,
		const VulkanDeviceSettings& settings)
	{
		std::vector<const char*> extensions = settings.required_extensions;
		for (const char* optional_extension : settings.optional_extensions)
		{
			if (!VulkanDeviceSupportsExtension(physical_device, optional_extension)) continue;
			const auto existing = std::ranges::find_if(extensions, [optional_extension](const char* enabled_extension)
			{
				return std::strcmp(enabled_extension, optional_extension) == 0;
			});
			if (existing == extensions.end()) extensions.push_back(optional_extension);
		}
		return extensions;
	}
}
