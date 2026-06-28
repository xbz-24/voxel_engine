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
	}
	bool VulkanGpuChunkRenderer::Record(VkCommandBuffer command_buffer,
		std::uint32_t image_index,
		const Camera& camera,
		VulkanOverlayRecordCallback overlay_callback,
		void* overlay_user_data)
	{
		if (!initialized_ || image_index >= framebuffers_.size()) return false;
		VkClearValue clear_color{};
		clear_color.color = { { 0.34f, 0.50f, 0.68f, 1.0f } };
		VkClearValue clear_depth{};
		clear_depth.depthStencil = { 1.0f, 0u };
		std::array clear_values{ clear_color, clear_depth };

		VkRenderPassBeginInfo begin_info{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		begin_info.renderPass = render_pass_;
		begin_info.framebuffer = framebuffers_[image_index];
		begin_info.renderArea.extent = extent_;
		begin_info.clearValueCount = static_cast<std::uint32_t>(clear_values.size());
		begin_info.pClearValues = clear_values.data();
		vkCmdBeginRenderPass(command_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.width = static_cast<float>(extent_.width);
		viewport.height = static_cast<float>(extent_.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{};
		scissor.extent = extent_;
		vkCmdSetViewport(command_buffer, 0u, 1u, &viewport);
		vkCmdSetScissor(command_buffer, 0u, 1u, &scissor);
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);

		const float aspect = static_cast<float>(extent_.width) / static_cast<float>(std::max(extent_.height, 1u));
		glm::mat4 projection = glm::perspective(glm::radians(72.0f), aspect, 0.05f, FarWorldClipDistance);
		projection[1][1] *= -1.0f;
		const glm::mat4 mvp = projection * camera.GetViewMatrix();
		vkCmdPushConstants(command_buffer, pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT, 0u, sizeof(glm::mat4), &mvp);

		if (index_count_ > 0u)
		{
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
