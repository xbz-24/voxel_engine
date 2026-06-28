#include "VulkanGpuChunkRenderer.h"

#include "VulkanBackend.h"

#include <array>

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreateSwapchainResources()
	{
		ReleaseSwapchainResources();
		extent_ = backend_->Swapchain().Extent();
		if (extent_.width == 0u || extent_.height == 0u) return false;
		const auto& images = backend_->Swapchain().Images();
		swapchain_image_views_.resize(images.size());
		for (std::size_t index = 0; index < images.size(); ++index)
		{
			VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			view_info.image = images[index];
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = color_format_;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.levelCount = 1u;
			view_info.subresourceRange.layerCount = 1u;
			if (vkCreateImageView(device_, &view_info, nullptr, &swapchain_image_views_[index]) != VK_SUCCESS) return false;
		}
		return CreateDepthResources() && CreateFramebuffers();
	}
	bool VulkanGpuChunkRenderer::CreateDepthResources()
	{
		VkImageCreateInfo image_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.format = depth_format_;
		image_info.extent = { extent_.width, extent_.height, 1u };
		image_info.mipLevels = 1u;
		image_info.arrayLayers = 1u;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if (!CreateDeviceImage(image_info, depth_memory_, depth_image_)) return false;

		VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		view_info.image = depth_image_;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = depth_format_;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		view_info.subresourceRange.levelCount = 1u;
		view_info.subresourceRange.layerCount = 1u;
		return vkCreateImageView(device_, &view_info, nullptr, &depth_view_) == VK_SUCCESS;
	}
	bool VulkanGpuChunkRenderer::CreateFramebuffers()
	{
		framebuffers_.resize(swapchain_image_views_.size());
		for (std::size_t index = 0; index < swapchain_image_views_.size(); ++index)
		{
			std::array attachments{ swapchain_image_views_[index], depth_view_ };
			VkFramebufferCreateInfo framebuffer_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebuffer_info.renderPass = render_pass_;
			framebuffer_info.attachmentCount = static_cast<std::uint32_t>(attachments.size());
			framebuffer_info.pAttachments = attachments.data();
			framebuffer_info.width = extent_.width;
			framebuffer_info.height = extent_.height;
			framebuffer_info.layers = 1u;
			if (vkCreateFramebuffer(device_, &framebuffer_info, nullptr, &framebuffers_[index]) != VK_SUCCESS) return false;
		}
		return true;
	}
}
