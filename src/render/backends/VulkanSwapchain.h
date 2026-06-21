#pragma once

#include "CoreTypes.h"

#include <volk.h>
#include <vulkan/vulkan.hpp>

namespace ve::rendering
{
	/** Owns the Vulkan swapchain and images used for presentation. */
	class VulkanSwapchain
	{
	public:
		/** Releases the swapchain when destroyed. */
		~VulkanSwapchain();

		/** @param physical_device Selected GPU. @param device Logical device. @param surface Presentation surface. @param width Window width. @param height Window height. @return True when created. */
		[[nodiscard]] bool Create(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, int width, int height);

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
