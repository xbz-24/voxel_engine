#include "VulkanGpuChunkRenderer.h"

#include "VulkanBackend.h"

namespace ve::rendering
{
	void VulkanGpuChunkRenderer::ReleaseMeshBuffers()
	{
		if (vertex_buffer_ != VK_NULL_HANDLE) vkDestroyBuffer(device_, vertex_buffer_, nullptr);
		if (vertex_memory_ != VK_NULL_HANDLE) vkFreeMemory(device_, vertex_memory_, nullptr);
		if (index_buffer_ != VK_NULL_HANDLE) vkDestroyBuffer(device_, index_buffer_, nullptr);
		if (index_memory_ != VK_NULL_HANDLE) vkFreeMemory(device_, index_memory_, nullptr);
		vertex_buffer_ = VK_NULL_HANDLE;
		vertex_memory_ = VK_NULL_HANDLE;
		index_buffer_ = VK_NULL_HANDLE;
		index_memory_ = VK_NULL_HANDLE;
		index_count_ = 0u;
		mesh_valid_ = false;
		mesh_revision_ = 0u;
	}
	void VulkanGpuChunkRenderer::ReleaseSwapchainResources()
	{
		for (VkFramebuffer framebuffer : framebuffers_)
		{
			if (framebuffer != VK_NULL_HANDLE) vkDestroyFramebuffer(device_, framebuffer, nullptr);
		}
		framebuffers_.clear();
		if (depth_view_ != VK_NULL_HANDLE) vkDestroyImageView(device_, depth_view_, nullptr);
		if (depth_image_ != VK_NULL_HANDLE) vkDestroyImage(device_, depth_image_, nullptr);
		if (depth_memory_ != VK_NULL_HANDLE) vkFreeMemory(device_, depth_memory_, nullptr);
		depth_view_ = VK_NULL_HANDLE;
		depth_image_ = VK_NULL_HANDLE;
		depth_memory_ = VK_NULL_HANDLE;
		for (VkImageView image_view : swapchain_image_views_)
		{
			if (image_view != VK_NULL_HANDLE) vkDestroyImageView(device_, image_view, nullptr);
		}
		swapchain_image_views_.clear();
		extent_ = {};
	}
	void VulkanGpuChunkRenderer::ReleasePipelineResources()
	{
		if (pipeline_ != VK_NULL_HANDLE) vkDestroyPipeline(device_, pipeline_, nullptr);
		if (pipeline_layout_ != VK_NULL_HANDLE) vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
		if (render_pass_ != VK_NULL_HANDLE) vkDestroyRenderPass(device_, render_pass_, nullptr);
		pipeline_ = VK_NULL_HANDLE;
		pipeline_layout_ = VK_NULL_HANDLE;
		render_pass_ = VK_NULL_HANDLE;
	}
	void VulkanGpuChunkRenderer::Release()
	{
		if (device_ != VK_NULL_HANDLE) vkDeviceWaitIdle(device_);
		ReleaseMeshBuffers();
		ReleaseSwapchainResources();
		ReleasePipelineResources();
		backend_ = nullptr;
		device_ = VK_NULL_HANDLE;
		physical_device_ = VK_NULL_HANDLE;
		command_pool_ = VK_NULL_HANDLE;
		color_format_ = VK_FORMAT_UNDEFINED;
		initialized_ = false;
	}
}
