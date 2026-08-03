		/** @return GPU memory allocator used by Vulkan render resources. */
		[[nodiscard]] VulkanMemoryAllocator& Allocator() noexcept;

		/** @return Vulkan swapchain used for presentation. */
		[[nodiscard]] VulkanSwapchain& Swapchain() noexcept;

		/** @return Optional Vulkan object-label helper. */
		[[nodiscard]] VulkanDebugLabels& DebugLabels() noexcept;

		/** @return True when the backend owns a Vulkan instance. */
		[[nodiscard]] bool IsInitialized() const noexcept;

	private:
		VulkanContext context_;
		VulkanSurface surface_;
		VulkanPhysicalDevice physical_device_;
		VulkanDevice device_;
		VulkanMemoryAllocator allocator_;
		VulkanSwapchain swapchain_;
		VulkanDebugLabels debug_labels_;
	};
}
