#include "VulkanFrameRenderer.h"

#include "VulkanBackend.h"

namespace ve::rendering
{
	void VulkanFrameRenderer::Release()
	{
		if (device_ != VK_NULL_HANDLE) vkDeviceWaitIdle(device_);
		ReleaseIntermediateImages();
		for (FrameResources& frame : frames_) frame.upload_buffer.Release(device_);
		if (timestamp_query_pool_ != VK_NULL_HANDLE) vkDestroyQueryPool(device_, timestamp_query_pool_, nullptr);
		timestamp_query_pool_ = VK_NULL_HANDLE;
		timestamp_period_ns_ = 0.0f;
		for (FrameResources& frame : frames_) frame.timestamp_query_valid = false;
		for (FrameResources& frame : frames_)
		{
			if (frame.in_flight != VK_NULL_HANDLE) vkDestroyFence(device_, frame.in_flight, nullptr);
			frame.in_flight = VK_NULL_HANDLE;
		}
		for (FrameResources& frame : frames_)
		{
			if (frame.render_finished != VK_NULL_HANDLE) vkDestroySemaphore(device_, frame.render_finished, nullptr);
			frame.render_finished = VK_NULL_HANDLE;
		}
		images_in_flight_.clear();
		for (FrameResources& frame : frames_)
		{
			if (frame.image_available != VK_NULL_HANDLE) vkDestroySemaphore(device_, frame.image_available, nullptr);
			frame.image_available = VK_NULL_HANDLE;
		}
		if (command_pool_ != VK_NULL_HANDLE) vkDestroyCommandPool(device_, command_pool_, nullptr);
		command_pool_ = VK_NULL_HANDLE;
		for (FrameResources& frame : frames_) frame.command_buffer = VK_NULL_HANDLE;
		image_layouts_.clear();
		imgui_overlay_.Release();
		gpu_chunk_renderer_.Release();
		rasterizer_.Release();
		previous_frame_timing_ = {};
		demo_settings_ = {};
		device_ = VK_NULL_HANDLE;
		backend_ = nullptr;
		current_frame_ = 0;
		logged_first_frame_ = false;
	}
}
