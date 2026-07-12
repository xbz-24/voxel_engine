#include "VulkanGpuChunkRenderer.h"

#include <cstring>

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreateShaderFrameResources(std::size_t frame_resource_count)
	{
		if (frame_resource_count == 0u) return false;

		VkDescriptorSetLayoutBinding frame_binding{};
		frame_binding.binding = 0u;
		frame_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		frame_binding.descriptorCount = 1u;
		frame_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layout_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layout_info.bindingCount = 1u;
		layout_info.pBindings = &frame_binding;
		if (vkCreateDescriptorSetLayout(device_, &layout_info, nullptr, &shader_descriptor_set_layout_) != VK_SUCCESS)
		{
			return false;
		}

		VkDescriptorPoolSize pool_size{};
		pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_size.descriptorCount = RenderElementCount(frame_resource_count);
		VkDescriptorPoolCreateInfo pool_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		pool_info.maxSets = RenderElementCount(frame_resource_count);
		pool_info.poolSizeCount = 1u;
		pool_info.pPoolSizes = &pool_size;
		if (vkCreateDescriptorPool(device_, &pool_info, nullptr, &shader_descriptor_pool_) != VK_SUCCESS)
		{
			return false;
		}

		shader_frame_resources_.resize(frame_resource_count);
		for (ShaderFrameResources& frame : shader_frame_resources_)
		{
			if (!CreateHostBuffer(
				VulkanByteSize(sizeof(VulkanVoxelFrameUniforms)),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				frame.uniform_buffer,
				frame.uniform_memory) ||
				vkMapMemory(
					device_,
					frame.uniform_memory,
					0u,
					VulkanByteSize(sizeof(VulkanVoxelFrameUniforms)),
					0u,
					&frame.mapped_uniform_data) != VK_SUCCESS)
			{
				return false;
			}
		}

		std::vector<VkDescriptorSetLayout> layouts(frame_resource_count, shader_descriptor_set_layout_);
		std::vector<VkDescriptorSet> descriptor_sets(frame_resource_count, VK_NULL_HANDLE);
		VkDescriptorSetAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocate_info.descriptorPool = shader_descriptor_pool_;
		allocate_info.descriptorSetCount = RenderElementCount(layouts.size());
		allocate_info.pSetLayouts = layouts.data();
		if (vkAllocateDescriptorSets(device_, &allocate_info, descriptor_sets.data()) != VK_SUCCESS)
		{
			return false;
		}

		for (std::size_t index = 0; index < shader_frame_resources_.size(); ++index)
		{
			ShaderFrameResources& frame = shader_frame_resources_[index];
			frame.descriptor_set = descriptor_sets[index];
			VkDescriptorBufferInfo buffer_info{};
			buffer_info.buffer = frame.uniform_buffer;
			buffer_info.range = sizeof(VulkanVoxelFrameUniforms);
			VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
			write.dstSet = frame.descriptor_set;
			write.dstBinding = 0u;
			write.descriptorCount = 1u;
			write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write.pBufferInfo = &buffer_info;
			vkUpdateDescriptorSets(device_, 1u, &write, 0u, nullptr);
		}
		return true;
	}

	bool VulkanGpuChunkRenderer::UpdateShaderFrameUniforms(
		std::size_t frame_index,
		const VulkanVoxelFrameUniforms& frame_uniforms) const
	{
		if (frame_index >= shader_frame_resources_.size()) return false;
		void* mapped_uniform_data = shader_frame_resources_[frame_index].mapped_uniform_data;
		if (mapped_uniform_data == nullptr) return false;
		std::memcpy(mapped_uniform_data, &frame_uniforms, sizeof(frame_uniforms));
		return true;
	}

	void VulkanGpuChunkRenderer::ReleaseShaderFrameResources()
	{
		if (shader_descriptor_pool_ != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(device_, shader_descriptor_pool_, nullptr);
		}
		shader_descriptor_pool_ = VK_NULL_HANDLE;
		for (ShaderFrameResources& frame : shader_frame_resources_)
		{
			if (frame.mapped_uniform_data != nullptr) vkUnmapMemory(device_, frame.uniform_memory);
			if (frame.uniform_buffer != VK_NULL_HANDLE) vkDestroyBuffer(device_, frame.uniform_buffer, nullptr);
			if (frame.uniform_memory != VK_NULL_HANDLE) vkFreeMemory(device_, frame.uniform_memory, nullptr);
			frame = {};
		}
		shader_frame_resources_.clear();
		if (shader_descriptor_set_layout_ != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(device_, shader_descriptor_set_layout_, nullptr);
		}
		shader_descriptor_set_layout_ = VK_NULL_HANDLE;
	}
}
