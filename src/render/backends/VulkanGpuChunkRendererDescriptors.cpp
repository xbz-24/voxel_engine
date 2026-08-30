#include "VulkanGpuChunkRenderer.h"

#include <cstring>

namespace ve::rendering
{
	bool VulkanGpuChunkRendererResourceOperations::CreateShaderFrameResources(
		std::size_t frame_resource_count)
	{
		if (frame_resource_count == 0u) return false;
		return CreateShaderDescriptorInfrastructure(frame_resource_count) &&
			CreateMappedShaderFrames(frame_resource_count) &&
			AllocateShaderDescriptorSets();
	}

	bool VulkanGpuChunkRendererResourceOperations::UpdateShaderFrameUniforms(
		std::size_t frame_index,
		const VulkanVoxelFrameUniforms& frame_uniforms) const
	{
		if (frame_index >= shader_frame_resources_.size()) return false;
		void* mapped_uniform_data = shader_frame_resources_[frame_index].mapped_uniform_data;
		if (mapped_uniform_data == nullptr) return false;
		std::memcpy(mapped_uniform_data, &frame_uniforms, sizeof(frame_uniforms));
		return true;
	}

	void VulkanGpuChunkRendererResourceOperations::ReleaseShaderFrameResources()
	{
		if (shader_descriptor_pool_ != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(device_, shader_descriptor_pool_, nullptr);
		}
		shader_descriptor_pool_ = VK_NULL_HANDLE;
		for (VulkanGpuShaderFrameResources& frame : shader_frame_resources_)
		{
			if (frame.shadow_framebuffer != VK_NULL_HANDLE) vkDestroyFramebuffer(device_, frame.shadow_framebuffer, nullptr);
			if (frame.shadow_view != VK_NULL_HANDLE) vkDestroyImageView(device_, frame.shadow_view, nullptr);
			if (frame.shadow_image != VK_NULL_HANDLE) vkDestroyImage(device_, frame.shadow_image, nullptr);
			if (frame.shadow_memory != VK_NULL_HANDLE) vkFreeMemory(device_, frame.shadow_memory, nullptr);
			if (frame.mapped_uniform_data != nullptr) vkUnmapMemory(device_, frame.uniform_memory);
			if (frame.uniform_buffer != VK_NULL_HANDLE) vkDestroyBuffer(device_, frame.uniform_buffer, nullptr);
			if (frame.uniform_memory != VK_NULL_HANDLE) vkFreeMemory(device_, frame.uniform_memory, nullptr);
			frame = {};
		}
		shader_frame_resources_.clear();
		if (shadow_sampler_ != VK_NULL_HANDLE) vkDestroySampler(device_, shadow_sampler_, nullptr);
		shadow_sampler_ = VK_NULL_HANDLE;
		if (shader_descriptor_set_layout_ != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(device_, shader_descriptor_set_layout_, nullptr);
		}
		shader_descriptor_set_layout_ = VK_NULL_HANDLE;
	}
}
