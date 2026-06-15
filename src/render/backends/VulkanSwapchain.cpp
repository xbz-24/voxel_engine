#include "VulkanSwapchain.h"

#include "VulkanSwapchainChoices.h"

#include <algorithm>

namespace ve::rendering
{
	/** Releases the Vulkan swapchain. */
	VulkanSwapchain::~VulkanSwapchain() { Release(); }

	/** Creates a Vulkan swapchain for the selected device and surface. */
	bool VulkanSwapchain::Create(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, int width, int height)
	{
		Release();
		device_ = device;
		const VulkanSwapchainSupportDetails support = QuerySwapchainSupport(physical_device, surface);
		if (!support.IsComplete()) return false;
		const VkSurfaceFormatKHR surface_format = ChooseSwapchainSurfaceFormat(support.formats);
		const VkPresentModeKHR present_mode = ChooseSwapchainPresentMode(support.present_modes);
		extent_ = ChooseSwapchainExtent(support.capabilities, width, height);
		std::uint32_t image_count = support.capabilities.minImageCount + 1;
		if (support.capabilities.maxImageCount > 0) image_count = std::min(image_count, support.capabilities.maxImageCount);
		VkSwapchainCreateInfoKHR create_info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		create_info.surface = surface;
		create_info.minImageCount = image_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = extent_;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.preTransform = support.capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;
		if (vkCreateSwapchainKHR(device_, &create_info, nullptr, &swapchain_) != VK_SUCCESS) return false;
		image_format_ = surface_format.format;
		vkGetSwapchainImagesKHR(device_, swapchain_, &image_count, nullptr);
		images_.resize(image_count);
		vkGetSwapchainImagesKHR(device_, swapchain_, &image_count, images_.data());
		return true;
	}

	/** Destroys the swapchain. */
	void VulkanSwapchain::Release()
	{
		if (swapchain_ != VK_NULL_HANDLE) vkDestroySwapchainKHR(device_, swapchain_, nullptr);
		swapchain_ = VK_NULL_HANDLE;
		device_ = VK_NULL_HANDLE;
		images_.clear();
	}

	/** Returns the swapchain handle. */
	VkSwapchainKHR VulkanSwapchain::Handle() const noexcept { return swapchain_; }

	/** Returns the Vulkan-Hpp swapchain handle. */
	vk::SwapchainKHR VulkanSwapchain::CppHandle() const noexcept { return vk::SwapchainKHR{ swapchain_ }; }

	/** Returns the swapchain image format. */
	VkFormat VulkanSwapchain::ImageFormat() const noexcept { return image_format_; }

	/** Returns the swapchain extent. */
	VkExtent2D VulkanSwapchain::Extent() const noexcept { return extent_; }

	/** Returns swapchain image handles. */
	const ve::core::DynamicArray<VkImage>& VulkanSwapchain::Images() const noexcept { return images_; }
}
