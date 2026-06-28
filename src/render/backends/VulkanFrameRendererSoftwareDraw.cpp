#include "VulkanFrameRenderer.h"

#include "Camera.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "World.h"

#include <chrono>

namespace ve::rendering
{
	bool VulkanFrameRenderer::DrawSoftwareFrame(const ve::world::World& world,
		const Camera& camera,
		int displayed_fps,
		double delta_seconds,
		const VulkanDemoInput& input)
	{
		if (backend_ == nullptr || device_ == VK_NULL_HANDLE) return false;
		const VkFence fence = frames_[current_frame_].in_flight;
		if (vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) return false;
		VulkanFrameTiming completed_frame_timing = previous_frame_timing_;
		CaptureCompletedGpuTiming(current_frame_, completed_frame_timing);

		const VkExtent2D extent = backend_->Swapchain().Extent();
		if (!EnsureFrameBuffer(extent)) return false;
		rasterizer_.Render(VulkanSoftwareVoxelRasterizerFrame{ world, camera, extent, backend_->Swapchain().ImageFormat(), displayed_fps, delta_seconds, completed_frame_timing, input, demo_settings_ });
		VulkanFrameTiming current_timing = rasterizer_.LastTiming();
		current_timing.gpu_copy_ms = completed_frame_timing.gpu_copy_ms;
		current_timing.has_gpu_copy_timing = completed_frame_timing.has_gpu_copy_timing;
		if (!UploadFramePixels(current_timing, current_frame_)) return false;

		std::uint32_t image_index = 0;
		const auto present_start = std::chrono::steady_clock::now();
		const VkResult acquire_result = vkAcquireNextImageKHR(device_, backend_->Swapchain().Handle(), UINT64_MAX,
			frames_[current_frame_].image_available, VK_NULL_HANDLE, &image_index);
		if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) return false;
		if (image_index >= image_layouts_.size()) return false;
		if (image_index >= images_in_flight_.size()) return false;
		const VkFence image_fence = images_in_flight_[image_index];
		if (image_fence != VK_NULL_HANDLE && vkWaitForFences(device_, 1u, &image_fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) return false;

		if (vkResetFences(device_, 1, &fence) != VK_SUCCESS) return false;
		VkCommandBuffer command_buffer = frames_[current_frame_].command_buffer;
		if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS || !RecordSoftwareCommandBuffer(command_buffer, image_index, current_frame_))
		{
			return false;
		}

		const VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		const VkSemaphore image_available = frames_[current_frame_].image_available;
		const VkSemaphore render_finished = frames_[current_frame_].render_finished;
		VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &image_available;
		submit_info.pWaitDstStageMask = &wait_stage;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &render_finished;
		if (vkQueueSubmit(backend_->Device().GraphicsQueue(), 1, &submit_info, fence) != VK_SUCCESS) return false;
		images_in_flight_[image_index] = fence;

		VkPresentInfoKHR present_info{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &render_finished;
		const VkSwapchainKHR swapchain = backend_->Swapchain().Handle();
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &swapchain;
		present_info.pImageIndices = &image_index;
		const VkResult present_result = vkQueuePresentKHR(backend_->Device().PresentQueue(), &present_info);
		const auto present_end = std::chrono::steady_clock::now();
		current_timing.present_cpu_ms = std::chrono::duration<double, std::milli>(present_end - present_start).count();
		if (timestamp_query_pool_ != VK_NULL_HANDLE) frames_[current_frame_].timestamp_query_valid = true;
		previous_frame_timing_ = current_timing;
		current_frame_ = (current_frame_ + 1) % kFramesInFlight;
		if (!logged_first_frame_)
		{
			VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Presented first Vulkan voxel frame");
			logged_first_frame_ = true;
		}
		return present_result == VK_SUCCESS || present_result == VK_SUBOPTIMAL_KHR;
	}
}
