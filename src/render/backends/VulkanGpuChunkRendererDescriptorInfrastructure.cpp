#include "VulkanGpuChunkRenderer.h"

#include <array>

namespace ve::rendering
{
	bool VulkanGpuChunkRendererResourceOperations::CreateShaderDescriptorInfrastructure(
		std::size_t frame_count)
	{
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
		if (vkCreateDescriptorSetLayout(device_, &layout_info, nullptr,
			&shader_descriptor_set_layout_) != VK_SUCCESS) return false;

		std::array<VkDescriptorPoolSize, 2> pool_sizes{};
		pool_sizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, RenderElementCount(frame_count) };
		pool_sizes[1] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, RenderElementCount(frame_count) };
		VkDescriptorPoolCreateInfo pool_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		pool_info.maxSets = RenderElementCount(frame_count);
		pool_info.poolSizeCount = RenderElementCount(pool_sizes.size());
		pool_info.pPoolSizes = pool_sizes.data();
		if (vkCreateDescriptorPool(device_, &pool_info, nullptr,
			&shader_descriptor_pool_) != VK_SUCCESS) return false;

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
		return vkCreateSampler(device_, &sampler_info, nullptr, &shadow_sampler_) == VK_SUCCESS;
	}
}
