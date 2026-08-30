#include "VulkanGpuChunkRenderer.h"

#include <array>

namespace ve::rendering
{
	bool VulkanGpuChunkRendererResourceOperations::AllocateShaderDescriptorSets()
	{
		const std::size_t frame_count = shader_frame_resources_.size();
		std::vector<VkDescriptorSetLayout> layouts(frame_count, shader_descriptor_set_layout_);
		std::vector<VkDescriptorSet> descriptor_sets(frame_count, VK_NULL_HANDLE);
		VkDescriptorSetAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocate_info.descriptorPool = shader_descriptor_pool_;
		allocate_info.descriptorSetCount = RenderElementCount(layouts.size());
		allocate_info.pSetLayouts = layouts.data();
		if (vkAllocateDescriptorSets(device_, &allocate_info, descriptor_sets.data()) != VK_SUCCESS)
		{
			return false;
		}

		for (std::size_t index = 0; index < frame_count; ++index)
		{
			VulkanGpuShaderFrameResources& frame = shader_frame_resources_[index];
			frame.descriptor_set = descriptor_sets[index];
			VkDescriptorBufferInfo buffer_info{};
			buffer_info.buffer = frame.uniform_buffer;
			buffer_info.range = sizeof(VulkanVoxelFrameUniforms);
			VkDescriptorImageInfo image_info{};
			image_info.sampler = shadow_sampler_;
			image_info.imageView = frame.shadow_view;
			image_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			std::array writes{
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
}
