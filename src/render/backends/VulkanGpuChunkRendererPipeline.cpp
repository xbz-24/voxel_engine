#include "VulkanGpuChunkRenderer.h"

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreatePipeline(const std::filesystem::path& shader_directory)
	{
		const VkShaderModule vertex_shader = CreateShaderModule(shader_directory / "voxel_chunk.vert.spv");
		const VkShaderModule fragment_shader = CreateShaderModule(shader_directory / "voxel_chunk.frag.spv");
		if (vertex_shader == VK_NULL_HANDLE || fragment_shader == VK_NULL_HANDLE)
		{
			if (vertex_shader != VK_NULL_HANDLE) vkDestroyShaderModule(device_, vertex_shader, nullptr);
			if (fragment_shader != VK_NULL_HANDLE) vkDestroyShaderModule(device_, fragment_shader, nullptr);
			return false;
		}

		const bool created = CreatePipelineLayout() && CreateGraphicsPipeline(vertex_shader, fragment_shader);
		vkDestroyShaderModule(device_, vertex_shader, nullptr);
		vkDestroyShaderModule(device_, fragment_shader, nullptr);
		return created;
	}
}
