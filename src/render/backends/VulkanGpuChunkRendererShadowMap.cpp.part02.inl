			1u,
			&frame.descriptor_set,
			0u,
			nullptr);
		if (shadow_index_count_ > 0u)
		{
			const VkDeviceSize vertex_offset = 0u;
			vkCmdBindVertexBuffers(command_buffer, 0u, 1u, &vertex_buffer_, &vertex_offset);
			vkCmdBindIndexBuffer(command_buffer, shadow_index_buffer_, 0u, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(command_buffer, shadow_index_count_, 1u, 0u, 0, 0u);
		}
		vkCmdEndRenderPass(command_buffer);
		return true;
	}
}
