#pragma once

#include "CoreTypes.h"

#include <volk.h>
#include <vulkan/vulkan.hpp>

#include <cstdint>

namespace ve::rendering
{
	struct VulkanSwapchainSettings
	{
		int width = 0;
		int height = 0;
		bool enable_vsync = false;
		std::uint32_t extra_image_count = 1;
		std::uint32_t image_array_layers = 1;
		VkImageUsageFlags image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		VkBool32 clipped = VK_TRUE;
	};

	/** Owns the Vulkan swapchain and images used for presentation. */
	class VulkanSwapchain
	{
	public:
		/** Releases the swapchain when destroyed. */
		~VulkanSwapchain();

		/** @param physical_device Selected GPU. @param device Logical device. @param surface Presentation surface. @param width Window width. @param height Window height. @param is_vsync_enabled Whether presentation should wait for vblank. @return True when created. */
		[[nodiscard]] bool Create(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, int width, int height, bool is_vsync_enabled = false);

		/** @param physical_device Selected GPU. @param device Logical device. @param surface Presentation surface. @param settings Swapchain settings. @return True when created. */
		[[nodiscard]] bool Create(
			VkPhysicalDevice physical_device,
			VkDevice device,
			VkSurfaceKHR surface,
			const VulkanSwapchainSettings& settings);

		/** Destroys the swapchain and clears image handles. */
		void Release();

		/** @return Vulkan swapchain handle. */
		[[nodiscard]] VkSwapchainKHR Handle() const noexcept;

		/** @return C++ Vulkan-Hpp swapchain handle. */
		[[nodiscard]] vk::SwapchainKHR CppHandle() const noexcept;

		/** @return Swapchain image format. */
		[[nodiscard]] VkFormat ImageFormat() const noexcept;

		/** @return Swapchain image extent. */
		[[nodiscard]] VkExtent2D Extent() const noexcept;

		/** @return Presentation images owned by the swapchain. */
		[[nodiscard]] const ve::core::DynamicArray<VkImage>& Images() const noexcept;

	private:
		VkDevice device_ = VK_NULL_HANDLE;
		VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
		VkFormat image_format_ = VK_FORMAT_UNDEFINED;
		VkExtent2D extent_{};
		ve::core::DynamicArray<VkImage> images_;
	};
}
