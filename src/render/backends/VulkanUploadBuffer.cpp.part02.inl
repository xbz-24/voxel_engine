	}

	void VulkanUploadBuffer::Release(VkDevice device)
	{
		if (device != VK_NULL_HANDLE && memory_ != VK_NULL_HANDLE && mapped_memory_ != nullptr) vkUnmapMemory(device, memory_);
		if (device != VK_NULL_HANDLE && buffer_ != VK_NULL_HANDLE) vkDestroyBuffer(device, buffer_, nullptr);
		if (device != VK_NULL_HANDLE && memory_ != VK_NULL_HANDLE) vkFreeMemory(device, memory_, nullptr);
		buffer_ = VK_NULL_HANDLE;
		memory_ = VK_NULL_HANDLE;
		size_ = 0;
		mapped_memory_ = nullptr;
	}
}
