#include "VulkanGpuChunkRenderer.h"

#include <array>

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreatePipeline(const std::filesystem::path& shader_directory)
	{
		constexpr std::array<const char*, 4> shader_file_names{{
			"voxel_chunk.vert.spv",
			"voxel_chunk.frag.spv",
			"voxel_sky.vert.spv",
			"voxel_sky.frag.spv"
		}};
		std::array<VkShaderModule, shader_file_names.size()> shader_modules{};
		bool loaded_all_shaders = true;
		for (std::size_t index = 0; index < shader_file_names.size(); ++index)
		{
			shader_modules[index] = CreateShaderModule(shader_directory / shader_file_names[index]);
			loaded_all_shaders = loaded_all_shaders && shader_modules[index] != VK_NULL_HANDLE;
		}

		const GraphicsPipelineSettings voxel_settings{
			.vertex_layout = VertexLayout::Voxel,
			.depth_test_enabled = true,
			.depth_write_enabled = true,
			.alpha_blending_enabled = true
		};
		const GraphicsPipelineSettings sky_settings{};
		const bool created = loaded_all_shaders &&
			CreatePipelineLayout() &&
			CreateGraphicsPipeline(shader_modules[0], shader_modules[1], voxel_settings, voxel_pipeline_) &&
			CreateGraphicsPipeline(shader_modules[2], shader_modules[3], sky_settings, sky_pipeline_);
		for (VkShaderModule shader_module : shader_modules)
		{
			if (shader_module != VK_NULL_HANDLE) vkDestroyShaderModule(device_, shader_module, nullptr);
		}
		return created;
	}
}
