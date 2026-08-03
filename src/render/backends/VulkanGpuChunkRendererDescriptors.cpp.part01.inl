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
