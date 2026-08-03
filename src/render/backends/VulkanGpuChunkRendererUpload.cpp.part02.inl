		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandBufferCount = 1u;
		if (vkAllocateCommandBuffers(device_, &allocate_info, &command_buffer) != VK_SUCCESS) return false;
		const auto free_command_buffer = [&]
		{
			if (command_buffer != VK_NULL_HANDLE) vkFreeCommandBuffers(device_, command_pool_, 1u, &command_buffer);
		};

		VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
		{
			free_command_buffer();
			return false;
		}
		record(command_buffer, user_data);
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
		{
			free_command_buffer();
			return false;
		}

		VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit_info.commandBufferCount = 1u;
		submit_info.pCommandBuffers = &command_buffer;
		const bool submitted = vkQueueSubmit(backend_->Device().GraphicsQueue(), 1u, &submit_info, VK_NULL_HANDLE) == VK_SUCCESS;
		if (submitted) vkQueueWaitIdle(backend_->Device().GraphicsQueue());
		free_command_buffer();
		return submitted;
	}
}
