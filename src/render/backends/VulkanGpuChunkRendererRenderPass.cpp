#include "VulkanGpuChunkRenderer.h"

#include "VulkanBackend.h"

#include <array>

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreateRenderPass()
	{
		color_format_ = backend_->Swapchain().ImageFormat();
		VkAttachmentDescription color_attachment{};
		color_attachment.format = color_format_;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depth_attachment{};
		depth_attachment.format = depth_format_;
		depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		const VkAttachmentReference color_reference{ 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		const VkAttachmentReference depth_reference{ 1u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1u;
		subpass.pColorAttachments = &color_reference;
		subpass.pDepthStencilAttachment = &depth_reference;

		std::array attachments{ color_attachment, depth_attachment };
		VkRenderPassCreateInfo render_pass_info{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		render_pass_info.attachmentCount = static_cast<std::uint32_t>(attachments.size());
		render_pass_info.pAttachments = attachments.data();
		render_pass_info.subpassCount = 1u;
		render_pass_info.pSubpasses = &subpass;
		return vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_) == VK_SUCCESS;
	}
}
