	void VulkanImGuiOverlay::Record(VkCommandBuffer command_buffer)
	{
		if (!initialized_ || !has_draw_data_) return;
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
	}

	bool VulkanImGuiOverlay::WantsMouseInput() const noexcept
	{
		return initialized_ && ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse;
	}

	bool VulkanImGuiOverlay::WantsKeyboardInput() const noexcept
	{
		if (!initialized_ || ImGui::GetCurrentContext() == nullptr) return false;
		const ImGuiIO& io = ImGui::GetIO();
		return io.WantCaptureKeyboard || io.WantTextInput || ImGui::IsAnyItemActive();
	}

	bool VulkanImGuiOverlay::IsInitialized() const noexcept
	{
		return initialized_;
	}

	void VulkanImGuiOverlay::Release()
	{
		if (device_ != VK_NULL_HANDLE) vkDeviceWaitIdle(device_);
		if (initialized_)
		{
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
		device_ = VK_NULL_HANDLE;
		initialized_ = false;
		has_draw_data_ = false;
	}
}
