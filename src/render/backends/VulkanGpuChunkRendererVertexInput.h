#pragma once

#include "VulkanGpuChunkRendererPipelineTypes.h"

#include <array>

namespace ve::rendering::detail
{
	struct VulkanGpuVertexInputDescriptions
	{
		void Configure(VulkanGpuVertexLayout layout) noexcept;

		VkVertexInputBindingDescription binding{};
		std::array<VkVertexInputAttributeDescription, 4> attributes{};
		VkPipelineVertexInputStateCreateInfo create_info{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
		};
	};
}
