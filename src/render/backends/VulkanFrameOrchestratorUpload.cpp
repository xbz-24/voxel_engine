#include "VulkanFrameOrchestrator.h"

#include "Logger.h"
#include "VulkanBackend.h"

#include <chrono>
#include <span>
#include <string>

namespace ve::rendering
{
	bool VulkanFrameOrchestrator::EnsureFrameBuffer(VkExtent2D extent)
	{
		if (!rasterizer_.Resize(extent, demo_settings_)) return false;
		const VkExtent2D render_extent = rasterizer_.RenderExtent();
		const VkDeviceSize byte_size = static_cast<VkDeviceSize>(render_extent.width) * static_cast<VkDeviceSize>(render_extent.height) * sizeof(std::uint32_t);
		const bool size_changed = frames_.front().upload_buffer.Size() != byte_size;
		for (FrameResources& frame : frames_)
		{
			if (!frame.upload_buffer.Ensure(*backend_, byte_size)) return false;
		}
		if (!EnsureIntermediateImages(render_extent, backend_->Swapchain().ImageFormat())) return false;
		if (size_changed)
		{
			VE_LOG_CATEGORY_INFO(ve::log::category::Render, std::string("Allocated Vulkan CPU upload framebuffer: ") +
				std::to_string(render_extent.width) + "x" + std::to_string(render_extent.height));
		}
		return true;
	}
	bool VulkanFrameOrchestrator::UploadFramePixels(VulkanFrameTiming& timing, std::size_t frame_index)
	{
		if (frame_index >= frames_.size()) return false;
		const std::span<const std::uint32_t> pixels = rasterizer_.RenderPixels();
		const auto start = std::chrono::steady_clock::now();
		const bool copied = frames_[frame_index].upload_buffer.CopyFrom(device_, pixels.data(), pixels.size_bytes());
		const auto end = std::chrono::steady_clock::now();
		timing.upload_cpu_ms = std::chrono::duration<double, std::milli>(end - start).count();
		return copied;
	}
}
