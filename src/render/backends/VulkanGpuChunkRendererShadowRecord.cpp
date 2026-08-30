#include "VulkanGpuChunkRenderer.h"

namespace ve::rendering
{
	bool VulkanGpuChunkRendererResourceOperations::RecordShadowPass(
		VkCommandBuffer command_buffer,
		std::size_t frame_index) const
	{
		if (frame_index >= shader_frame_resources_.size() || shadow_pipeline_ == VK_NULL_HANDLE)
		{
			return false;
		}
		const VulkanGpuShaderFrameResources& frame = shader_frame_resources_[frame_index];
		if (frame.shadow_framebuffer == VK_NULL_HANDLE || frame.descriptor_set == VK_NULL_HANDLE)
		{
			return false;
		}

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
		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline_layout_, 0u, 1u, &frame.descriptor_set, 0u, nullptr);
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
