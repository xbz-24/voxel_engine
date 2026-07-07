#include "VulkanFrameOrchestrator.h"

#include "Camera.h"
#include "Logger.h"
#include "VulkanFrameOrchestratorCommands.h"

namespace ve::rendering
{
	bool VulkanFrameOrchestrator::RecordGpuCommandBuffer(VkCommandBuffer command_buffer, std::uint32_t image_index, std::size_t frame_index, const Camera& camera)
	{
		VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) return false;

		const std::uint32_t first_query = static_cast<std::uint32_t>(frame_index * 2u);
		if (timestamp_query_pool_ != VK_NULL_HANDLE)
		{
			vkCmdResetQueryPool(command_buffer, timestamp_query_pool_, first_query, 2u);
			vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, timestamp_query_pool_, first_query);
		}
		const VulkanOverlayRecordCallback overlay_callback = imgui_overlay_.IsInitialized() ? RecordImguiOverlay : nullptr;
		void* overlay_user_data = imgui_overlay_.IsInitialized() ? &imgui_overlay_ : nullptr;
		if (!gpu_chunk_renderer_.Record(command_buffer, image_index, camera, overlay_callback, overlay_user_data))
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to record Vulkan GPU chunk commands");
			return false;
		}
		if (timestamp_query_pool_ != VK_NULL_HANDLE)
		{
			vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, timestamp_query_pool_, first_query + 1u);
		}
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to end Vulkan GPU command buffer");
			return false;
		}
		return true;
	}
	void VulkanFrameOrchestrator::CaptureCompletedGpuTiming(std::size_t frame_index, VulkanFrameTiming& timing) const
	{
		if (timestamp_query_pool_ == VK_NULL_HANDLE || timestamp_period_ns_ <= 0.0f) return;
		if (!frames_[frame_index].timestamp_query_valid) return;
		std::array<std::uint64_t, 2> timestamps{};
		const VkResult result = vkGetQueryPoolResults(device_,
			timestamp_query_pool_,
			static_cast<std::uint32_t>(frame_index * 2u),
			static_cast<std::uint32_t>(timestamps.size()),
			sizeof(std::uint64_t) * timestamps.size(),
			timestamps.data(),
			sizeof(std::uint64_t),
			VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
		if (result != VK_SUCCESS || timestamps[1] <= timestamps[0]) return;
		const double elapsed_ns = static_cast<double>(timestamps[1] - timestamps[0]) * static_cast<double>(timestamp_period_ns_);
		timing.gpu_copy_ms = elapsed_ns / 1'000'000.0;
		timing.has_gpu_copy_timing = true;
	}
}
