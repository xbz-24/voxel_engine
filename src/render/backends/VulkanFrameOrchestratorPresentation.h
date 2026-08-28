#pragma once

#include "VulkanFrameTypes.h"

#include <volk.h>

#include <cstdint>
#include <span>

namespace ve::rendering
{
	class VulkanBackend;

	[[nodiscard]] VulkanFrameResult ClassifyVulkanPresentationResult(
		VkResult result) noexcept;
	[[nodiscard]] bool VulkanAcquireCanContinue(VkResult result) noexcept;
	[[nodiscard]] VulkanFrameResult CombineVulkanFrameResults(
		VulkanFrameResult first,
		VulkanFrameResult second) noexcept;

	[[nodiscard]] VkResult AcquireSwapchainImage(VkDevice device,
		VulkanBackend& backend,
		VkSemaphore image_available,
		std::uint32_t& image_index) noexcept;
	[[nodiscard]] bool WaitForSwapchainImage(VkDevice device,
		std::span<const VkFence> images_in_flight,
		std::uint32_t image_index) noexcept;
	[[nodiscard]] VkResult SubmitFrame(VkQueue queue,
		VkCommandBuffer command_buffer,
		VkPipelineStageFlags wait_stage,
		VkSemaphore image_available,
		VkSemaphore render_finished,
		VkFence fence) noexcept;
	[[nodiscard]] VkResult PresentSwapchainImage(VulkanBackend& backend,
		VkSemaphore render_finished,
		std::uint32_t image_index) noexcept;
}
