#include "VulkanRenderView.h"

namespace ve::engine
{
	/** Stores backend-owned Vulkan-Hpp handles without taking ownership. */
	VulkanRenderView::VulkanRenderView(VulkanRenderViewCreateInfo create_info) noexcept
		: create_info_(create_info)
	{
	}

	/** Returns the API represented by this view. */
	ve::rendering::GraphicsApi VulkanRenderView::Api() const noexcept { return ve::rendering::GraphicsApi::Vulkan; }

	/** Returns the non-owning Vulkan logical device handle. */
	vk::Device VulkanRenderView::Device() const noexcept { return create_info_.device; }

	/** Returns the non-owning Vulkan swapchain handle. */
	vk::SwapchainKHR VulkanRenderView::Swapchain() const noexcept { return create_info_.swapchain; }

	/** Returns the cached swapchain image extent. */
	vk::Extent2D VulkanRenderView::SwapchainExtent() const noexcept { return create_info_.swapchain_extent; }
}
