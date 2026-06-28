#include "VulkanFrameRenderer.h"
#include "VulkanFrameRendererPresentation.h"

#include "VulkanBackend.h"

#include <array>

namespace ve::rendering
{
	VkResult AcquireSwapchainImage(VkDevice device,
		VulkanBackend& backend,
		VkSemaphore image_available,
		std::uint32_t& image_index) noexcept
	{
		return vkAcquireNextImageKHR(device,
			backend.Swapchain().Handle(),
			UINT64_MAX,
			image_available,
			VK_NULL_HANDLE,
			&image_index);
	}

	bool WaitForSwapchainImage(VkDevice device, std::span<const VkFence> images_in_flight, std::uint32_t image_index) noexcept
	{
		if (image_index >= images_in_flight.size()) return false;
		const VkFence image_fence = images_in_flight[image_index];
		return image_fence == VK_NULL_HANDLE ||
			vkWaitForFences(device, 1u, &image_fence, VK_TRUE, UINT64_MAX) == VK_SUCCESS;
	}

	VkResult SubmitFrame(VkQueue queue,
		VkCommandBuffer command_buffer,
		VkPipelineStageFlags wait_stage,
		VkSemaphore image_available,
		VkSemaphore render_finished,
		VkFence fence) noexcept
	{
		VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit_info.waitSemaphoreCount = 1u;
		submit_info.pWaitSemaphores = &image_available;
		submit_info.pWaitDstStageMask = &wait_stage;
		submit_info.commandBufferCount = 1u;
		submit_info.pCommandBuffers = &command_buffer;
		submit_info.signalSemaphoreCount = 1u;
		submit_info.pSignalSemaphores = &render_finished;
		return vkQueueSubmit(queue, 1u, &submit_info, fence);
	}

	VkResult PresentSwapchainImage(VulkanBackend& backend, VkSemaphore render_finished, std::uint32_t image_index) noexcept
	{
		VkPresentInfoKHR present_info{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		present_info.waitSemaphoreCount = 1u;
		present_info.pWaitSemaphores = &render_finished;
		const VkSwapchainKHR swapchain = backend.Swapchain().Handle();
		present_info.swapchainCount = 1u;
		present_info.pSwapchains = &swapchain;
		present_info.pImageIndices = &image_index;
		return vkQueuePresentKHR(backend.Device().PresentQueue(), &present_info);
	}

	bool VulkanFrameRenderer::WaitForAllInFlightFrames() const
	{
		if (device_ == VK_NULL_HANDLE) return false;
		std::array<VkFence, kFramesInFlight> fences{};
		for (std::size_t index = 0; index < frames_.size(); ++index)
		{
			fences[index] = frames_[index].in_flight;
		}
		return vkWaitForFences(device_,
			static_cast<std::uint32_t>(fences.size()),
			fences.data(),
			VK_TRUE,
			UINT64_MAX) == VK_SUCCESS;
	}
}
