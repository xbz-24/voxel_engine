#include "VulkanGpuChunkRenderer.h"

namespace ve::rendering
{
	bool VulkanGpuChunkRendererResourceOperations::CreateShadowFrameResources(
		VulkanGpuShaderFrameResources& frame) const
	{
		if (shadow_render_pass_ == VK_NULL_HANDLE) return false;
		VkFormatProperties format_properties{};
		vkGetPhysicalDeviceFormatProperties(physical_device_, depth_format_, &format_properties);
		constexpr VkFormatFeatureFlags RequiredFeatures =
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
		if ((format_properties.optimalTilingFeatures & RequiredFeatures) != RequiredFeatures) return false;

		VkImageCreateInfo image_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.format = depth_format_;
		image_info.extent = {
			VulkanDirectionalShadowMapResolution,
			VulkanDirectionalShadowMapResolution,
			1u
		};
		image_info.mipLevels = 1u;
		image_info.arrayLayers = 1u;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if (!CreateDeviceImage(image_info, frame.shadow_memory, frame.shadow_image)) return false;

		VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		view_info.image = frame.shadow_image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = depth_format_;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		view_info.subresourceRange.levelCount = 1u;
		view_info.subresourceRange.layerCount = 1u;
		if (vkCreateImageView(device_, &view_info, nullptr, &frame.shadow_view) != VK_SUCCESS) return false;

		VkFramebufferCreateInfo framebuffer_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		framebuffer_info.renderPass = shadow_render_pass_;
		framebuffer_info.attachmentCount = 1u;
		framebuffer_info.pAttachments = &frame.shadow_view;
		framebuffer_info.width = VulkanDirectionalShadowMapResolution;
		framebuffer_info.height = VulkanDirectionalShadowMapResolution;
		framebuffer_info.layers = 1u;
		return vkCreateFramebuffer(
			device_, &framebuffer_info, nullptr, &frame.shadow_framebuffer) == VK_SUCCESS;
	}
}
