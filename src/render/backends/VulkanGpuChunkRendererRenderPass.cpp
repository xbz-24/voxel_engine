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
		render_pass_info.attachmentCount = RenderElementCount(attachments.size());
		render_pass_info.pAttachments = attachments.data();
		render_pass_info.subpassCount = 1u;
		render_pass_info.pSubpasses = &subpass;
		if (vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS)
		{
			return false;
		}

		VkAttachmentDescription shadow_attachment{};
		shadow_attachment.format = depth_format_;
		shadow_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		shadow_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		shadow_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		shadow_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		shadow_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		shadow_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		shadow_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		const VkAttachmentReference shadow_reference{
			0u,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};
		VkSubpassDescription shadow_subpass{};
		shadow_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		shadow_subpass.pDepthStencilAttachment = &shadow_reference;

		std::array<VkSubpassDependency, 2> shadow_dependencies{};
		shadow_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		shadow_dependencies[0].dstSubpass = 0u;
		shadow_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		shadow_dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		shadow_dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		shadow_dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		shadow_dependencies[1].srcSubpass = 0u;
		shadow_dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		shadow_dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		shadow_dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		shadow_dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		shadow_dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		shadow_dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo shadow_render_pass_info{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		shadow_render_pass_info.attachmentCount = 1u;
		shadow_render_pass_info.pAttachments = &shadow_attachment;
		shadow_render_pass_info.subpassCount = 1u;
		shadow_render_pass_info.pSubpasses = &shadow_subpass;
		shadow_render_pass_info.dependencyCount = RenderElementCount(shadow_dependencies.size());
		shadow_render_pass_info.pDependencies = shadow_dependencies.data();
		return vkCreateRenderPass(device_, &shadow_render_pass_info, nullptr, &shadow_render_pass_) == VK_SUCCESS;
	}
}
