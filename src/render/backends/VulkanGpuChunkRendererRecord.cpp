#include "VulkanGpuChunkRenderer.h"

#include "VulkanBackend.h"

#include "Camera.h"

#include <algorithm>
#include <array>
#include <glm/ext.hpp>

namespace ve::rendering
{
	namespace
	{
		constexpr float FarWorldClipDistance = 1024.0f;
		constexpr VkShaderStageFlags VoxelShaderStages =
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

		void PushVoxelShaderConstants(
			VkCommandBuffer command_buffer,
			VkPipelineLayout pipeline_layout,
			const glm::mat4& transform)
		{
			const VulkanVoxelTransformPushConstants push_constants{ transform };
			vkCmdPushConstants(command_buffer,
				pipeline_layout,
				VoxelShaderStages,
				0u,
				sizeof(push_constants),
				&push_constants);
		}
	}
	bool VulkanGpuChunkRenderer::Record(VkCommandBuffer command_buffer,
		std::uint32_t image_index,
		std::size_t frame_index,
		float elapsed_seconds,
		const Camera& camera,
		VulkanOverlayRecordCallback overlay_callback,
		void* overlay_user_data)
	{
		if (!initialized_ || image_index >= framebuffers_.size() ||
			frame_index >= shader_frame_resources_.size() ||
			voxel_pipeline_ == VK_NULL_HANDLE || sky_pipeline_ == VK_NULL_HANDLE) return false;
		const VulkanVoxelFrameUniforms frame_uniforms = PackVulkanVoxelFrameUniforms(
			render_style_,
			camera.GetPosition(),
			camera.GetForward(),
			elapsed_seconds,
			extent_);
		if (!UpdateShaderFrameUniforms(frame_index, frame_uniforms)) return false;
		if (!RecordShadowPass(command_buffer, frame_index)) return false;
		VkClearValue clear_color{};
		clear_color.color = { {
			frame_uniforms.sky_horizon_color_and_fog_start.x,
			frame_uniforms.sky_horizon_color_and_fog_start.y,
			frame_uniforms.sky_horizon_color_and_fog_start.z,
			1.0f } };
		VkClearValue clear_depth{};
		clear_depth.depthStencil = { 1.0f, 0u };
		std::array clear_values{ clear_color, clear_depth };

		VkRenderPassBeginInfo begin_info{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		begin_info.renderPass = render_pass_;
		begin_info.framebuffer = framebuffers_[image_index];
		begin_info.renderArea.extent = extent_;
		begin_info.clearValueCount = RenderElementCount(clear_values.size());
		begin_info.pClearValues = clear_values.data();
		vkCmdBeginRenderPass(command_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.width = ExtentFloat(extent_.width);
		viewport.height = ExtentFloat(extent_.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{};
		scissor.extent = extent_;
		vkCmdSetViewport(command_buffer, 0u, 1u, &viewport);
		vkCmdSetScissor(command_buffer, 0u, 1u, &scissor);
		const VkDescriptorSet frame_descriptor_set = shader_frame_resources_[frame_index].descriptor_set;
		vkCmdBindDescriptorSets(command_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline_layout_,
			0u,
			1u,
			&frame_descriptor_set,
			0u,
			nullptr);
		const float aspect = ExtentFloat(extent_.width) / ExtentFloat(std::max(extent_.height, 1u));
		glm::mat4 projection = glm::perspectiveRH_ZO(
			glm::radians(72.0f),
			aspect,
			0.05f,
			FarWorldClipDistance);
		projection[1][1] *= -1.0f;
		const glm::mat4 world_to_view = camera.GetWorldToViewMatrix();
		const glm::mat4 sky_view = glm::mat4{ glm::mat3{ world_to_view } };
		const glm::mat4 inverse_sky_view_projection = glm::inverse(projection * sky_view);

		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sky_pipeline_);
		PushVoxelShaderConstants(command_buffer, pipeline_layout_, inverse_sky_view_projection);
		vkCmdDraw(command_buffer, 3u, 1u, 0u, 0u);

		if (index_count_ > 0u)
		{
			vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, voxel_pipeline_);
			PushVoxelShaderConstants(command_buffer, pipeline_layout_, projection * world_to_view);
			const VkDeviceSize offset = 0u;
			vkCmdBindVertexBuffers(command_buffer, 0u, 1u, &vertex_buffer_, &offset);
			vkCmdBindIndexBuffer(command_buffer, index_buffer_, 0u, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(command_buffer, index_count_, 1u, 0u, 0, 0u);
		}
		if (overlay_callback != nullptr) overlay_callback(command_buffer, overlay_user_data);
		vkCmdEndRenderPass(command_buffer);
		return true;
	}
}
