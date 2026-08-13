#include "VulkanGpuChunkRenderer.h"

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreateShadowFrameResources(ShaderFrameResources& frame) const
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
		return vkCreateFramebuffer(device_, &framebuffer_info, nullptr, &frame.shadow_framebuffer) == VK_SUCCESS;
	}

	bool VulkanGpuChunkRenderer::RecordShadowPass(
		VkCommandBuffer command_buffer,
		std::size_t frame_index) const
	{
		if (frame_index >= shader_frame_resources_.size() || shadow_pipeline_ == VK_NULL_HANDLE)
		{
			return false;
		}
		const ShaderFrameResources& frame = shader_frame_resources_[frame_index];
		if (frame.shadow_framebuffer == VK_NULL_HANDLE || frame.descriptor_set == VK_NULL_HANDLE) return false;

		VkClearValue clear_depth{};
		clear_depth.depthStencil = { 1.0f, 0u };
		VkRenderPassBeginInfo begin_info{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		begin_info.renderPass = shadow_render_pass_;
		begin_info.framebuffer = frame.shadow_framebuffer;
		begin_info.renderArea.extent = {
			VulkanDirectionalShadowMapResolution,
			VulkanDirectionalShadowMapResolution
		};
		begin_info.clearValueCount = 1u;
		begin_info.pClearValues = &clear_depth;
		vkCmdBeginRenderPass(command_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.width = ve::core::ToFloat(VulkanDirectionalShadowMapResolution);
		viewport.height = ve::core::ToFloat(VulkanDirectionalShadowMapResolution);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{};
		scissor.extent = begin_info.renderArea.extent;
		vkCmdSetViewport(command_buffer, 0u, 1u, &viewport);
		vkCmdSetScissor(command_buffer, 0u, 1u, &scissor);
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadow_pipeline_);
		vkCmdBindDescriptorSets(
			command_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline_layout_,
			0u,
			1u,
			&frame.descriptor_set,
			0u,
			nullptr);
		if (shadow_index_count_ > 0u)
		{
			const VkDeviceSize vertex_offset = 0u;
			vkCmdBindVertexBuffers(command_buffer, 0u, 1u, &vertex_buffer_, &vertex_offset);
			vkCmdBindIndexBuffer(command_buffer, shadow_index_buffer_, 0u, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(command_buffer, shadow_index_count_, 1u, 0u, 0, 0u);
		}
		vkCmdEndRenderPass(command_buffer);
		return true;
	}
}
