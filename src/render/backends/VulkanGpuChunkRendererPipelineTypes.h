#pragma once

#include <volk.h>

namespace ve::rendering
{
	enum class VulkanGpuVertexLayout
	{
		None,
		VoxelPosition,
		Voxel
	};

	struct VulkanGpuGraphicsPipelineSettings
	{
		VkRenderPass render_pass = VK_NULL_HANDLE;
		VulkanGpuVertexLayout vertex_layout = VulkanGpuVertexLayout::None;
		bool depth_test_enabled = false;
		bool depth_write_enabled = false;
		bool alpha_blending_enabled = false;
		bool color_attachment_enabled = true;
		bool depth_bias_enabled = false;
	};
}
