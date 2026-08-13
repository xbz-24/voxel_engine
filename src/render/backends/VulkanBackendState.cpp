#include "VulkanBackend.h"

namespace ve::rendering
{
	void VulkanBackend::Release()
	{
		debug_labels_.Release();
		swapchain_.Release();
		allocator_.Release();
		device_.Release();
		surface_.Release();
		context_.Release();
	}

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

	GraphicsApi VulkanBackend::Api() const noexcept { return GraphicsApi::Vulkan; }
	VulkanContext& VulkanBackend::Context() noexcept { return context_; }
	VulkanSurface& VulkanBackend::Surface() noexcept { return surface_; }
	VulkanPhysicalDevice& VulkanBackend::PhysicalDevice() noexcept { return physical_device_; }
	VulkanDevice& VulkanBackend::Device() noexcept { return device_; }
	VulkanMemoryAllocator& VulkanBackend::Allocator() noexcept { return allocator_; }
	VulkanSwapchain& VulkanBackend::Swapchain() noexcept { return swapchain_; }
	VulkanDebugLabels& VulkanBackend::DebugLabels() noexcept { return debug_labels_; }
	bool VulkanBackend::IsInitialized() const noexcept { return context_.IsInitialized(); }
}
