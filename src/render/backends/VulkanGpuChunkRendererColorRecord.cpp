#include "VulkanGpuChunkRenderer.h"

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
			const VulkanVoxelTransformPushConstants constants{ transform };
			vkCmdPushConstants(command_buffer, pipeline_layout, VoxelShaderStages,
				0u, sizeof(constants), &constants);
		}
	}

	bool VulkanGpuChunkRendererResourceOperations::RecordColorPass(
		VkCommandBuffer command_buffer,
		std::uint32_t image_index,
		std::size_t frame_index,
		const VulkanVoxelFrameUniforms& uniforms,
		const Camera& camera,
		VulkanGpuOverlayRecordCallback overlay_callback,
		void* overlay_user_data) const
	{
		VkClearValue clear_color{};
		clear_color.color = { {
			uniforms.sky_horizon_color_and_fog_start.x,
			uniforms.sky_horizon_color_and_fog_start.y,
			uniforms.sky_horizon_color_and_fog_start.z,
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
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{};
		scissor.extent = extent_;
		vkCmdSetViewport(command_buffer, 0u, 1u, &viewport);
		vkCmdSetScissor(command_buffer, 0u, 1u, &scissor);
		const VkDescriptorSet descriptor_set = shader_frame_resources_[frame_index].descriptor_set;
		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline_layout_, 0u, 1u, &descriptor_set, 0u, nullptr);

		const float aspect = ExtentFloat(extent_.width) /
			ExtentFloat(std::max(extent_.height, 1u));
		glm::mat4 projection = glm::perspectiveRH_ZO(
			glm::radians(72.0f), aspect, 0.05f, FarWorldClipDistance);
		projection[1][1] *= -1.0f;
		const glm::mat4 world_to_view = camera.GetWorldToViewMatrix();
		const glm::mat4 sky_view = glm::mat4{ glm::mat3{ world_to_view } };
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sky_pipeline_);
		PushVoxelShaderConstants(command_buffer, pipeline_layout_,
			glm::inverse(projection * sky_view));
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
