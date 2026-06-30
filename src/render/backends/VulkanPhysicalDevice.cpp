#include "VulkanPhysicalDevice.h"

#include "CoreTypes.h"
#include "VulkanDeviceExtensions.h"
#include "VulkanSwapchainSupport.h"

namespace ve::rendering
{
	namespace
	{
		/** @param device GPU to query. @param surface Surface requiring presentation. @return Graphics/present queue indices. */
		VulkanQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
		{
			std::uint32_t count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
			ve::core::DynamicArray<VkQueueFamilyProperties> families(count);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());
			VulkanQueueFamilyIndices indices{};
			for (std::uint32_t index = 0; index < count; ++index)
			{
				if ((families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) indices.graphics_family = index;
				VkBool32 supports_present = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &supports_present);
				if (supports_present == VK_TRUE) indices.present_family = index;
				if (indices.IsComplete()) break;
			}
			return indices;
		}

		bool DeviceMatchesCriteria(
			VkPhysicalDevice candidate,
			const VulkanPhysicalDeviceSelectionCriteria& criteria,
			VulkanQueueFamilyIndices& queue_families)
		{
			if (criteria.surface == VK_NULL_HANDLE) return false;
			if (!VulkanDeviceSupportsExtensions(candidate, criteria.required_device_extensions)) return false;
			queue_families = FindQueueFamilies(candidate, criteria.surface);
			if (!queue_families.IsComplete()) return false;
			return !criteria.settings.require_swapchain_support ||
				QuerySwapchainSupport(candidate, criteria.surface).IsComplete();
		}
	}

	/** Reports whether all required queue families were found. */
	bool VulkanQueueFamilyIndices::IsComplete() const noexcept
	{
		return graphics_family != UINT32_MAX && present_family != UINT32_MAX;
	}

	/** Selects the first GPU that can render and present. */
	bool VulkanPhysicalDevice::Select(VkInstance instance, VkSurfaceKHR surface)
	{
		VulkanPhysicalDeviceSelectionCriteria criteria{};
		criteria.surface = surface;
		return Select(instance, criteria);
	}

	bool VulkanPhysicalDevice::Select(VkInstance instance, const VulkanPhysicalDeviceSelectionCriteria& criteria)
	{
		std::uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
		if (device_count == 0) return false;
		ve::core::DynamicArray<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(instance, &device_count, devices.data());
		device_ = VK_NULL_HANDLE;
		queue_families_ = {};
		for (VkPhysicalDevice candidate : devices)
		{
			VulkanQueueFamilyIndices families{};
			if (!DeviceMatchesCriteria(candidate, criteria, families)) continue;
			device_ = candidate;
			queue_families_ = families;
			return true;
		}
		return false;
	}

	/** Returns the selected physical device. */
	VkPhysicalDevice VulkanPhysicalDevice::Handle() const noexcept { return device_; }

	/** Returns the Vulkan-Hpp physical device handle. */
	vk::PhysicalDevice VulkanPhysicalDevice::CppHandle() const noexcept { return vk::PhysicalDevice{ device_ }; }

	/** Returns queue family indices. */
	const VulkanQueueFamilyIndices& VulkanPhysicalDevice::QueueFamilies() const noexcept { return queue_families_; }
}
