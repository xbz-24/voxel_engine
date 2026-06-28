#include "VulkanFrameRenderer.h"

#include "Camera.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "VulkanFrameRendererPresentation.h"
#include "World.h"

#include <chrono>
#include <string>

namespace ve::rendering
{
	bool VulkanFrameRenderer::DrawGpuFrame(const ve::world::World& world,
		const Camera& camera,
		int displayed_fps,
		double delta_seconds,
		VulkanMinecraftDemoSettings& minecraft_demo_settings)
	{
		if (backend_ == nullptr || device_ == VK_NULL_HANDLE) return false;
		const VkFence fence = frames_[current_frame_].in_flight;
		if (vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) return false;
		VulkanFrameTiming completed_frame_timing = previous_frame_timing_;
		CaptureCompletedGpuTiming(current_frame_, completed_frame_timing);
		if (gpu_chunk_renderer_.NeedsWorldMeshUpdate(world) && !WaitForAllInFlightFrames()) return false;
		if (!gpu_chunk_renderer_.EnsureWorldMesh(world)) return false;
		imgui_overlay_.BeginFrame(minecraft_demo_settings, VulkanMinecraftDemoStats{
			displayed_fps,
			delta_seconds,
			completed_frame_timing.gpu_copy_ms,
			completed_frame_timing.present_cpu_ms,
			gpu_chunk_renderer_.IndexCount(),
			world.Revision(),
			completed_frame_timing.has_gpu_copy_timing,
			true
		});
		std::uint32_t image_index = 0;
		const auto present_start = std::chrono::steady_clock::now();
		const VkResult acquire_result = AcquireSwapchainImage(device_, *backend_, frames_[current_frame_].image_available, image_index);
		if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to acquire Vulkan swapchain image: " + std::to_string(static_cast<int>(acquire_result)));
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
		if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS || !RecordGpuCommandBuffer(command_buffer, image_index, current_frame_, camera))
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
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to submit Vulkan GPU frame: " + std::to_string(static_cast<int>(submit_result)));
			return false;
		}
		images_in_flight_[image_index] = fence;
		const VkResult present_result = PresentSwapchainImage(*backend_, render_finished, image_index);
		const auto present_end = std::chrono::steady_clock::now();

		VulkanFrameTiming current_timing{};
		current_timing.gpu_copy_ms = completed_frame_timing.gpu_copy_ms;
		current_timing.has_gpu_copy_timing = completed_frame_timing.has_gpu_copy_timing;
		current_timing.present_cpu_ms = std::chrono::duration<double, std::milli>(present_end - present_start).count();
		current_timing.render_extent = backend_->Swapchain().Extent();
		current_timing.sample_step = 1u;
		current_timing.worker_count = 0u;
		if (timestamp_query_pool_ != VK_NULL_HANDLE) frames_[current_frame_].timestamp_query_valid = true;
		previous_frame_timing_ = current_timing;
		current_frame_ = (current_frame_ + 1u) % kFramesInFlight;
		if (!logged_first_frame_)
		{
			VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Presented first Vulkan GPU chunk frame");
			logged_first_frame_ = true;
		}
		return present_result == VK_SUCCESS || present_result == VK_SUBOPTIMAL_KHR;
	}
}
