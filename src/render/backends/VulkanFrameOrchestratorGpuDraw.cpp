#include "VulkanFrameOrchestrator.h"

#include "CoreTypes.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "VulkanFrameOrchestratorPresentation.h"

#include <chrono>
#include <string>

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] int VulkanResultCode(VkResult result) noexcept
		{
			return ve::core::ToInt(result);
		}
	}

	bool VulkanFrameOrchestrator::DrawGpuFrame(const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		const Camera& camera,
		int displayed_fps,
		double delta_seconds,
		VulkanOverlaySettings& overlay_settings,
		const VulkanGpuFrameControls& controls)
	{
		if (backend_ == nullptr || device_ == VK_NULL_HANDLE) return false;
		ApplyGpuFrameControls(delta_seconds, overlay_settings, controls);
		const VkFence fence = frames_[current_frame_].in_flight;
		if (vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) return false;
		VulkanFrameTiming completed_frame_timing = previous_frame_timing_;
		CaptureCompletedGpuTiming(current_frame_, completed_frame_timing);
		if (!PrepareGpuScene(world,
			block_registry,
			displayed_fps,
			delta_seconds,
			overlay_settings,
			controls,
			completed_frame_timing)) return false;
		std::uint32_t image_index = 0;
		const auto present_start = std::chrono::steady_clock::now();
		const VkResult acquire_result = AcquireSwapchainImage(device_, *backend_, frames_[current_frame_].image_available, image_index);
		if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to acquire Vulkan swapchain image: " + std::to_string(VulkanResultCode(acquire_result)));
			return false;
		}
		if (!WaitForSwapchainImage(device_, images_in_flight_, image_index))
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to wait for Vulkan swapchain image fence");
			return false;
		}
		if (vkResetFences(device_, 1, &fence) != VK_SUCCESS)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to reset Vulkan frame fence");
			return false;
		}
		VkCommandBuffer command_buffer = frames_[current_frame_].command_buffer;
		if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS ||
			!RecordGpuCommandBuffer(
				command_buffer,
				image_index,
				current_frame_,
				ve::core::ToFloat(shader_elapsed_seconds_),
				camera))
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to prepare Vulkan GPU command buffer");
			return false;
		}
		const VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		const VkSemaphore image_available = frames_[current_frame_].image_available;
		const VkSemaphore render_finished = frames_[current_frame_].render_finished;
		const VkResult submit_result = SubmitFrame(backend_->Device().GraphicsQueue(),
			command_buffer,
			wait_stage,
			image_available,
			render_finished,
			fence);
		if (submit_result != VK_SUCCESS)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to submit Vulkan GPU frame: " + std::to_string(VulkanResultCode(submit_result)));
			return false;
		}
		images_in_flight_[image_index] = fence;
		const VkResult present_result = PresentSwapchainImage(*backend_, render_finished, image_index);
		const auto present_end = std::chrono::steady_clock::now();

		const double present_cpu_ms =
			std::chrono::duration<double, std::milli>(present_end - present_start).count();
		return CompleteGpuFrame(completed_frame_timing, present_cpu_ms, present_result);
	}
}
