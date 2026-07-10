#include "VulkanGpuChunkRenderer.h"

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreatePipelineLayout()
	{
		VkPushConstantRange push_constants{};
		push_constants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		push_constants.size = sizeof(VulkanVoxelPushConstants);

		VkPipelineLayoutCreateInfo layout_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		layout_info.pushConstantRangeCount = 1u;
		layout_info.pPushConstantRanges = &push_constants;
		return vkCreatePipelineLayout(device_, &layout_info, nullptr, &pipeline_layout_) == VK_SUCCESS;
	}
}
