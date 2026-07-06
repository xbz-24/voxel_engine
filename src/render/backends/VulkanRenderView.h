#pragma once

#include "RenderView.h"

#include <volk.h>
#include <vulkan/vulkan.hpp>

namespace ve::engine
{
	struct VulkanRenderViewCreateInfo
	{
		vk::Device device{};
		vk::SwapchainKHR swapchain{};
		vk::Extent2D swapchain_extent{};
	};

	/** Vulkan view adapter that exposes C++ Vulkan-Hpp handles to higher layers. */
	class VulkanRenderView final : public RenderView
	{
	public:
		/** @param create_info Vulkan-Hpp handles captured from the active backend. */
		explicit VulkanRenderView(VulkanRenderViewCreateInfo create_info) noexcept;

		/** @return API represented by this Vulkan view. */
		[[nodiscard]] ve::rendering::GraphicsApi Api() const noexcept override;

		/** @return Vulkan-Hpp logical device handle used by view-level render passes. */
		[[nodiscard]] vk::Device Device() const noexcept;

		/** @return Vulkan-Hpp swapchain handle used for presentation. */
		[[nodiscard]] vk::SwapchainKHR Swapchain() const noexcept;

		/** @return Current swapchain extent in pixels. */
		[[nodiscard]] vk::Extent2D SwapchainExtent() const noexcept;

	private:
		VulkanRenderViewCreateInfo create_info_;
	};
}
