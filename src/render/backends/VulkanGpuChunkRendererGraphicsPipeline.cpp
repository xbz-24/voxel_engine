#include "VulkanGpuChunkRenderer.h"

#include <array>
#include <cstddef>
#include <glm/mat4x4.hpp>

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreateGraphicsPipeline(
		VkShaderModule vertex_shader,
		VkShaderModule fragment_shader,
		const GraphicsPipelineSettings& settings,
		VkPipeline& output_pipeline) const
	{
		if (vertex_shader == VK_NULL_HANDLE || settings.render_pass == VK_NULL_HANDLE) return false;
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
		const bool has_fragment_shader = fragment_shader != VK_NULL_HANDLE;

		const bool uses_voxel_vertices = settings.vertex_layout != VertexLayout::None;
		const bool uses_position_only = settings.vertex_layout == VertexLayout::VoxelPosition;
		VkVertexInputBindingDescription binding{};
		binding.binding = 0u;
		binding.stride = sizeof(VoxelVertex);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		std::array attributes{
			VkVertexInputAttributeDescription{ 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, VertexAttributeOffset(offsetof(VoxelVertex, position)) },
			VkVertexInputAttributeDescription{ 1u, 0u, VK_FORMAT_R8G8B8A8_UNORM, VertexAttributeOffset(offsetof(VoxelVertex, color_rgba8)) },
			VkVertexInputAttributeDescription{ 2u, 0u, VK_FORMAT_R32_SFLOAT, VertexAttributeOffset(offsetof(VoxelVertex, light)) },
			VkVertexInputAttributeDescription{ 3u, 0u, VK_FORMAT_R8G8B8A8_SNORM, VertexAttributeOffset(offsetof(VoxelVertex, normal_snorm8)) }
		};
		VkPipelineVertexInputStateCreateInfo vertex_input{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		vertex_input.vertexBindingDescriptionCount = uses_voxel_vertices ? 1u : 0u;
		vertex_input.pVertexBindingDescriptions = uses_voxel_vertices ? &binding : nullptr;
		vertex_input.vertexAttributeDescriptionCount = uses_voxel_vertices
			? (uses_position_only ? 1u : RenderElementCount(attributes.size()))
			: 0u;
		vertex_input.pVertexAttributeDescriptions = uses_voxel_vertices ? attributes.data() : nullptr;

		VkPipelineInputAssemblyStateCreateInfo assembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		VkPipelineViewportStateCreateInfo viewport_state{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewport_state.viewportCount = 1u;
		viewport_state.scissorCount = 1u;
		std::array dynamic_states{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamic_state{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamic_state.dynamicStateCount = RenderElementCount(dynamic_states.size());
		dynamic_state.pDynamicStates = dynamic_states.data();

		VkPipelineRasterizationStateCreateInfo rasterization{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterization.polygonMode = VK_POLYGON_MODE_FILL;
		rasterization.cullMode = VK_CULL_MODE_NONE;
		rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterization.lineWidth = 1.0f;
		rasterization.depthBiasEnable = settings.depth_bias_enabled ? VK_TRUE : VK_FALSE;
		rasterization.depthBiasConstantFactor = settings.depth_bias_enabled ? 1.25f : 0.0f;
		rasterization.depthBiasSlopeFactor = settings.depth_bias_enabled ? 1.75f : 0.0f;
		VkPipelineMultisampleStateCreateInfo multisample{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		VkPipelineDepthStencilStateCreateInfo depth{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		depth.depthTestEnable = settings.depth_test_enabled ? VK_TRUE : VK_FALSE;
		depth.depthWriteEnable = settings.depth_write_enabled ? VK_TRUE : VK_FALSE;
		depth.depthCompareOp = settings.depth_test_enabled ? VK_COMPARE_OP_LESS : VK_COMPARE_OP_ALWAYS;

		VkPipelineColorBlendAttachmentState blend_attachment{};
		blend_attachment.blendEnable = settings.alpha_blending_enabled ? VK_TRUE : VK_FALSE;
		blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
		blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
		blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		VkPipelineColorBlendStateCreateInfo blend{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		blend.attachmentCount = settings.color_attachment_enabled ? 1u : 0u;
		blend.pAttachments = settings.color_attachment_enabled ? &blend_attachment : nullptr;

		VkGraphicsPipelineCreateInfo pipeline_info{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipeline_info.stageCount = has_fragment_shader ? RenderElementCount(shader_stages.size()) : 1u;
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
		pipeline_info.renderPass = settings.render_pass;
		return vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1u, &pipeline_info, nullptr, &output_pipeline) == VK_SUCCESS;
	}
}
