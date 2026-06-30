#include "VulkanDevice.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <vector>

namespace ve::rendering
{
	namespace
	{
		bool DeviceSupportsExtension(VkPhysicalDevice physical_device, const char* extension_name)
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

		bool AppendSupportedOptionalExtension(
			std::vector<const char*>& extensions,
			VkPhysicalDevice physical_device,
			const char* extension_name)
		{
			if (!DeviceSupportsExtension(physical_device, extension_name)) return false;
			const auto existing = std::ranges::find_if(extensions, [extension_name](const char* enabled_extension)
			{
				return std::strcmp(enabled_extension, extension_name) == 0;
			});
			if (existing == extensions.end()) extensions.push_back(extension_name);
			return true;
		}

		bool DeviceSupportsRequiredExtensions(
			VkPhysicalDevice physical_device,
			const VulkanDeviceSettings& settings)
		{
			return std::ranges::all_of(settings.required_extensions, [physical_device](const char* extension_name)
			{
				return DeviceSupportsExtension(physical_device, extension_name);
			});
		}

		std::vector<const char*> SelectEnabledDeviceExtensions(
			VkPhysicalDevice physical_device,
			const VulkanDeviceSettings& settings)
		{
			std::vector<const char*> extensions = settings.required_extensions;
			for (const char* optional_extension : settings.optional_extensions)
			{
				(void)AppendSupportedOptionalExtension(extensions, physical_device, optional_extension);
			}
			return extensions;
		}

		/** @param queue_family Queue family index. @param priority Queue priority storage. @return Queue create info. */
		VkDeviceQueueCreateInfo CreateQueueInfo(std::uint32_t queue_family, const float& priority) noexcept
		{
			VkDeviceQueueCreateInfo info{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
			info.queueFamilyIndex = queue_family;
			info.queueCount = 1;
			info.pQueuePriorities = &priority;
			return info;
		}
	}

	/** Releases the logical Vulkan device. */
	VulkanDevice::~VulkanDevice() { Release(); }

	/** Creates the logical device and stores queues. */
	bool VulkanDevice::Create(VkPhysicalDevice physical_device, const VulkanQueueFamilyIndices& queues)
	{
		return Create(physical_device, queues, VulkanDeviceSettings{});
	}

	bool VulkanDevice::Create(
		VkPhysicalDevice physical_device,
		const VulkanQueueFamilyIndices& queues,
		const VulkanDeviceSettings& settings)
	{
		Release();
		if (!DeviceSupportsRequiredExtensions(physical_device, settings)) return false;
		std::array queue_infos{
			CreateQueueInfo(queues.graphics_family, settings.queue_priority),
			CreateQueueInfo(queues.present_family, settings.queue_priority)
		};
		const bool same_queue = queues.graphics_family == queues.present_family;
		const std::vector<const char*> extensions = SelectEnabledDeviceExtensions(physical_device, settings);
		VkDeviceCreateInfo create_info{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		create_info.queueCreateInfoCount = same_queue ? 1u : 2u;
		create_info.pQueueCreateInfos = queue_infos.data();
		create_info.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();
		if (vkCreateDevice(physical_device, &create_info, nullptr, &device_) != VK_SUCCESS) return false;
		volkLoadDevice(device_);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vk::Device{ device_ });
		vkGetDeviceQueue(device_, queues.graphics_family, 0, &graphics_queue_);
		vkGetDeviceQueue(device_, queues.present_family, 0, &present_queue_);
		return true;
	}

	/** Destroys the logical device. */
	void VulkanDevice::Release()
	{
		if (device_ != VK_NULL_HANDLE) vkDestroyDevice(device_, nullptr);
		device_ = VK_NULL_HANDLE;
		graphics_queue_ = present_queue_ = VK_NULL_HANDLE;
	}

	/** Returns the logical Vulkan device. */
	VkDevice VulkanDevice::Handle() const noexcept { return device_; }

	/** Returns the Vulkan-Hpp logical device handle. */
	vk::Device VulkanDevice::CppHandle() const noexcept { return vk::Device{ device_ }; }

	/** Returns the graphics queue. */
	VkQueue VulkanDevice::GraphicsQueue() const noexcept { return graphics_queue_; }

	/** Returns the presentation queue. */
	VkQueue VulkanDevice::PresentQueue() const noexcept { return present_queue_; }
}
