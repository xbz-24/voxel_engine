#include "VulkanGpuChunkRenderer.h"

#include <array>
#include <cstddef>
#include <glm/mat4x4.hpp>

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreateGraphicsPipeline(VkShaderModule vertex_shader, VkShaderModule fragment_shader)
	{
		std::array shader_stages{
			VkPipelineShaderStageCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },
			VkPipelineShaderStageCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO }
		};
		shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shader_stages[0].module = vertex_shader;
		shader_stages[0].pName = "main";
		shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shader_stages[1].module = fragment_shader;
		shader_stages[1].pName = "main";

		VkVertexInputBindingDescription binding{};
		binding.binding = 0u;
		binding.stride = sizeof(VoxelVertex);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		std::array attributes{
			VkVertexInputAttributeDescription{ 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, static_cast<std::uint32_t>(offsetof(VoxelVertex, x)) },
			VkVertexInputAttributeDescription{ 1u, 0u, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<std::uint32_t>(offsetof(VoxelVertex, r)) },
			VkVertexInputAttributeDescription{ 2u, 0u, VK_FORMAT_R32_SFLOAT, static_cast<std::uint32_t>(offsetof(VoxelVertex, light)) }
		};
		VkPipelineVertexInputStateCreateInfo vertex_input{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		vertex_input.vertexBindingDescriptionCount = 1u;
		vertex_input.pVertexBindingDescriptions = &binding;
		vertex_input.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(attributes.size());
		vertex_input.pVertexAttributeDescriptions = attributes.data();

		VkPipelineInputAssemblyStateCreateInfo assembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		VkPipelineViewportStateCreateInfo viewport_state{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewport_state.viewportCount = 1u;
		viewport_state.scissorCount = 1u;
		std::array dynamic_states{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamic_state{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamic_state.dynamicStateCount = static_cast<std::uint32_t>(dynamic_states.size());
		dynamic_state.pDynamicStates = dynamic_states.data();

		VkPipelineRasterizationStateCreateInfo rasterization{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterization.polygonMode = VK_POLYGON_MODE_FILL;
		rasterization.cullMode = VK_CULL_MODE_NONE;
		rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterization.lineWidth = 1.0f;
		VkPipelineMultisampleStateCreateInfo multisample{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		VkPipelineDepthStencilStateCreateInfo depth{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		depth.depthTestEnable = VK_TRUE;
		depth.depthWriteEnable = VK_TRUE;
		depth.depthCompareOp = VK_COMPARE_OP_LESS;

		VkPipelineColorBlendAttachmentState blend_attachment{};
		blend_attachment.blendEnable = VK_TRUE;
		blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
		blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
		blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		VkPipelineColorBlendStateCreateInfo blend{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		blend.attachmentCount = 1u;
		blend.pAttachments = &blend_attachment;

		VkGraphicsPipelineCreateInfo pipeline_info{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipeline_info.stageCount = static_cast<std::uint32_t>(shader_stages.size());
		pipeline_info.pStages = shader_stages.data();
		pipeline_info.pVertexInputState = &vertex_input;
		pipeline_info.pInputAssemblyState = &assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterization;
		pipeline_info.pMultisampleState = &multisample;
		pipeline_info.pDepthStencilState = &depth;
		pipeline_info.pColorBlendState = &blend;
		pipeline_info.pDynamicState = &dynamic_state;
		pipeline_info.layout = pipeline_layout_;
		pipeline_info.renderPass = render_pass_;
		return vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1u, &pipeline_info, nullptr, &pipeline_) == VK_SUCCESS;
	}
}
