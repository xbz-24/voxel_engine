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
