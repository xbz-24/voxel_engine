		allocator_.Release();
		device_.Release();
		surface_.Release();
		context_.Release();
	}

	/** Returns capabilities expected from the Vulkan renderer path. */
	RenderBackendCapabilities VulkanBackend::Capabilities() const noexcept
	{
		return RenderBackendCapabilities{
			.compute = RenderFeatureSupport{ .supported = true, .required_major_version = 1 },
			.indirect_draw = RenderFeatureSupport{ .supported = true, .required_major_version = 1 },
			.ray_tracing = RenderFeatureSupport{
				.supported = false,
				.required_major_version = 1,
				.required_minor_version = 2,
				.required_extension = "VK_KHR_ray_tracing_pipeline"
			},
			.limits = RenderBackendLimits{ .max_texture_dimension_2d = 16384, .max_color_attachments = 8 },
			.is_available = context_.IsInitialized()
		};
	}

	/** Returns the Vulkan API identifier. */
	GraphicsApi VulkanBackend::Api() const noexcept
	{
		return GraphicsApi::Vulkan;
	}

	/** Returns the mutable Vulkan context. */
	VulkanContext& VulkanBackend::Context() noexcept { return context_; }

	/** Returns the Vulkan surface wrapper. */
	VulkanSurface& VulkanBackend::Surface() noexcept { return surface_; }

	/** Returns the selected physical device wrapper. */
	VulkanPhysicalDevice& VulkanBackend::PhysicalDevice() noexcept { return physical_device_; }

	/** Returns the logical device wrapper. */
	VulkanDevice& VulkanBackend::Device() noexcept { return device_; }

	/** Returns the Vulkan memory allocator wrapper. */
	VulkanMemoryAllocator& VulkanBackend::Allocator() noexcept { return allocator_; }

	/** Returns the Vulkan swapchain wrapper. */
	VulkanSwapchain& VulkanBackend::Swapchain() noexcept { return swapchain_; }

	/** Returns the optional Vulkan debug label helper. */
	VulkanDebugLabels& VulkanBackend::DebugLabels() noexcept { return debug_labels_; }

	/** Reports whether the Vulkan context is initialized. */
	bool VulkanBackend::IsInitialized() const noexcept { return context_.IsInitialized(); }
}
