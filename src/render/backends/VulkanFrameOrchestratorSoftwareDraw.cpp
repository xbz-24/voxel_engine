#include "VulkanFrameOrchestrator.h"

#include "Camera.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "VulkanFrameOrchestratorPresentation.h"
#include "World.h"

#include <chrono>

namespace ve::rendering
{
	VulkanFrameResult VulkanFrameOrchestrator::DrawSoftwareFrame(const ve::world::World& world,
		const Camera& camera,
		int displayed_fps,
		double delta_seconds,
		const VulkanFrameInput& input)
	{
		if (backend_ == nullptr || device_ == VK_NULL_HANDLE) return VulkanFrameResult::Failed;
		const VkFence fence = frames_[current_frame_].in_flight;
		if (vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) return VulkanFrameResult::Failed;
		VulkanFrameTiming completed_frame_timing = previous_frame_timing_;
		CaptureCompletedGpuTiming(current_frame_, completed_frame_timing);

		const VkExtent2D extent = backend_->Swapchain().Extent();
		if (!EnsureFrameBuffer(extent)) return VulkanFrameResult::Failed;
		rasterizer_.Render(VulkanSoftwareVoxelRasterizerFrame{ world, camera, extent, backend_->Swapchain().ImageFormat(), displayed_fps, delta_seconds, completed_frame_timing, input, software_rasterizer_settings_ });
		VulkanFrameTiming current_timing = rasterizer_.LastTiming();
		current_timing.gpu_copy_ms = completed_frame_timing.gpu_copy_ms;
		current_timing.has_gpu_copy_timing = completed_frame_timing.has_gpu_copy_timing;
		if (!UploadFramePixels(current_timing, current_frame_)) return VulkanFrameResult::Failed;

		std::uint32_t image_index = 0;
		const auto present_start = std::chrono::steady_clock::now();
		const VkResult acquire_result = AcquireSwapchainImage(
			device_, *backend_, frames_[current_frame_].image_available, image_index);
		const VulkanFrameResult acquire_frame_result = ClassifyVulkanPresentationResult(acquire_result);
		if (!VulkanAcquireCanContinue(acquire_result)) return acquire_frame_result;
		if (image_index >= image_layouts_.size()) return VulkanFrameResult::Failed;
		if (image_index >= images_in_flight_.size()) return VulkanFrameResult::Failed;
		const VkFence image_fence = images_in_flight_[image_index];
		if (image_fence != VK_NULL_HANDLE && vkWaitForFences(device_, 1u, &image_fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) return VulkanFrameResult::Failed;

		if (vkResetFences(device_, 1, &fence) != VK_SUCCESS) return VulkanFrameResult::Failed;
		VkCommandBuffer command_buffer = frames_[current_frame_].command_buffer;
		if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS || !RecordSoftwareCommandBuffer(command_buffer, image_index, current_frame_))
		{
			return VulkanFrameResult::Failed;
		}

		const VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		const VkSemaphore image_available = frames_[current_frame_].image_available;
		const VkSemaphore render_finished = frames_[current_frame_].render_finished;
		if (SubmitFrame(backend_->Device().GraphicsQueue(), command_buffer, wait_stage,
			image_available, render_finished, fence) != VK_SUCCESS) return VulkanFrameResult::Failed;
		images_in_flight_[image_index] = fence;

		const VkResult present_result = PresentSwapchainImage(*backend_, render_finished, image_index);
		const auto present_end = std::chrono::steady_clock::now();
		const VulkanFrameResult present_frame_result = ClassifyVulkanPresentationResult(present_result);
		current_timing.present_cpu_ms = std::chrono::duration<double, std::milli>(present_end - present_start).count();
		if (timestamp_query_pool_ != VK_NULL_HANDLE) frames_[current_frame_].timestamp_query_valid = true;
		previous_frame_timing_ = current_timing;
		current_frame_ = (current_frame_ + 1) % kFramesInFlight;
		if (!logged_first_frame_ && present_frame_result != VulkanFrameResult::Failed &&
			present_result != VK_ERROR_OUT_OF_DATE_KHR)
		{
			VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Presented first Vulkan voxel frame");
			logged_first_frame_ = true;
		}
		return CombineVulkanFrameResults(acquire_frame_result, present_frame_result);
	}
}
