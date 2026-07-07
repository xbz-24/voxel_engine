#pragma once

#include "VulkanSwapchainSupport.h"

#include <cstdint>
#include <span>

namespace ve::rendering
{
	/** @param formats Available surface formats. @return Preferred surface format. */
	[[nodiscard]] VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(std::span<const VkSurfaceFormatKHR> formats) noexcept;

	/** @param present_modes Available present modes. @param is_vsync_enabled Whether presentation should wait for vblank. @return Preferred present mode. */
	[[nodiscard]] VkPresentModeKHR ChooseSwapchainPresentMode(std::span<const VkPresentModeKHR> present_modes, bool is_vsync_enabled = false) noexcept;

	/** @param capabilities Surface capabilities. @param width Desired width. @param height Desired height. @return Valid image extent. */
	[[nodiscard]] VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height) noexcept;

	/** @param capabilities Surface capabilities. @param extra_image_count Images above the surface minimum. @return Valid swapchain image count. */
	[[nodiscard]] std::uint32_t ChooseSwapchainImageCount(
		const VkSurfaceCapabilitiesKHR& capabilities,
		std::uint32_t extra_image_count = 1) noexcept;
}
