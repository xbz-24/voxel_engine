	void VulkanContext::Release()
	{
		if (debug_messenger_ != VK_NULL_HANDLE)
		{
			vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
			debug_messenger_ = VK_NULL_HANDLE;
		}
		if (instance_ != VK_NULL_HANDLE) vkDestroyInstance(instance_, nullptr);
		instance_ = VK_NULL_HANDLE;
	}

	VkInstance VulkanContext::Instance() const noexcept { return instance_; }

	vk::Instance VulkanContext::CppInstance() const noexcept { return vk::Instance{ instance_ }; }

	bool VulkanContext::IsInitialized() const noexcept { return instance_ != VK_NULL_HANDLE; }
}
