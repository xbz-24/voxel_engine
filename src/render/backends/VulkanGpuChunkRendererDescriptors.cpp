#include "VulkanGpuChunkRenderer.h"

#include <array>
#include <cstring>

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::CreateShaderFrameResources(std::size_t frame_resource_count)
	{
		if (frame_resource_count == 0u) return false;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings{};
		bindings[0].binding = 0u;
		bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		bindings[0].descriptorCount = 1u;
		bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		bindings[1].binding = 1u;
		bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[1].descriptorCount = 1u;
		bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layout_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layout_info.bindingCount = RenderElementCount(bindings.size());
		layout_info.pBindings = bindings.data();
		if (vkCreateDescriptorSetLayout(device_, &layout_info, nullptr, &shader_descriptor_set_layout_) != VK_SUCCESS)
		{
			return false;
		}

		std::array<VkDescriptorPoolSize, 2> pool_sizes{};
		pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_sizes[0].descriptorCount = RenderElementCount(frame_resource_count);
		pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[1].descriptorCount = RenderElementCount(frame_resource_count);
		VkDescriptorPoolCreateInfo pool_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		pool_info.maxSets = RenderElementCount(frame_resource_count);
		pool_info.poolSizeCount = RenderElementCount(pool_sizes.size());
		pool_info.pPoolSizes = pool_sizes.data();
		if (vkCreateDescriptorPool(device_, &pool_info, nullptr, &shader_descriptor_pool_) != VK_SUCCESS)
		{
			return false;
		}

		VkSamplerCreateInfo sampler_info{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		sampler_info.magFilter = VK_FILTER_NEAREST;
		sampler_info.minFilter = VK_FILTER_NEAREST;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		sampler_info.compareEnable = VK_TRUE;
		sampler_info.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		sampler_info.maxLod = 0.0f;
		if (vkCreateSampler(device_, &sampler_info, nullptr, &shadow_sampler_) != VK_SUCCESS)
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
					&frame.mapped_uniform_data) != VK_SUCCESS ||
				!CreateShadowFrameResources(frame))
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
			VkDescriptorImageInfo image_info{};
			image_info.sampler = shadow_sampler_;
			image_info.imageView = frame.shadow_view;
			image_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			std::array<VkWriteDescriptorSet, 2> writes{
				VkWriteDescriptorSet{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET },
				VkWriteDescriptorSet{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET }
			};
			writes[0].dstSet = frame.descriptor_set;
			writes[0].dstBinding = 0u;
			writes[0].descriptorCount = 1u;
			writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writes[0].pBufferInfo = &buffer_info;
			writes[1].dstSet = frame.descriptor_set;
			writes[1].dstBinding = 1u;
			writes[1].descriptorCount = 1u;
			writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writes[1].pImageInfo = &image_info;
			vkUpdateDescriptorSets(device_, RenderElementCount(writes.size()), writes.data(), 0u, nullptr);
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
