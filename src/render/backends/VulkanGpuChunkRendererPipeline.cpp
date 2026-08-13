#include "VulkanGpuChunkRenderer.h"

#include <array>

namespace ve::rendering
{
	bool VulkanGpuChunkRendererResourceOperations::CreatePipeline(const std::filesystem::path& shader_directory)
	{
		constexpr std::array<const char*, 5> shader_file_names{{
			"voxel_chunk.vert.spv",
			"voxel_chunk.frag.spv",
			"voxel_sky.vert.spv",
			"voxel_sky.frag.spv",
			"voxel_shadow.vert.spv"
		}};
		std::array<VkShaderModule, shader_file_names.size()> shader_modules{};
		bool loaded_all_shaders = true;
		for (std::size_t index = 0; index < shader_file_names.size(); ++index)
		{
			shader_modules[index] = CreateShaderModule(shader_directory / shader_file_names[index]);
			loaded_all_shaders = loaded_all_shaders && shader_modules[index] != VK_NULL_HANDLE;
		}

		const VulkanGpuGraphicsPipelineSettings voxel_settings{
			.render_pass = render_pass_,
			.vertex_layout = VulkanGpuVertexLayout::Voxel,
			.depth_test_enabled = true,
			.depth_write_enabled = true,
			.alpha_blending_enabled = true
		};
		const VulkanGpuGraphicsPipelineSettings sky_settings{
			.render_pass = render_pass_
		};
		const VulkanGpuGraphicsPipelineSettings shadow_settings{
			.render_pass = shadow_render_pass_,
			.vertex_layout = VulkanGpuVertexLayout::VoxelPosition,
			.depth_test_enabled = true,
			.depth_write_enabled = true,
			.alpha_blending_enabled = false,
			.color_attachment_enabled = false,
			.depth_bias_enabled = true
		};
		const bool created = loaded_all_shaders &&
			CreatePipelineLayout() &&
			CreateGraphicsPipeline(shader_modules[0], shader_modules[1], voxel_settings, voxel_pipeline_) &&
			CreateGraphicsPipeline(shader_modules[2], shader_modules[3], sky_settings, sky_pipeline_) &&
			CreateGraphicsPipeline(shader_modules[4], VK_NULL_HANDLE, shadow_settings, shadow_pipeline_);
		for (VkShaderModule shader_module : shader_modules)
		{
			if (shader_module != VK_NULL_HANDLE) vkDestroyShaderModule(device_, shader_module, nullptr);
		}
		return created;
	}
}
