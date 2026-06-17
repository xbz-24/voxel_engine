#include "VulkanBackend.h"

#include "Logger.h"
#include "Window.h"

namespace ve::rendering
{
	/** Initializes Vulkan startup state. */
	bool VulkanBackend::Initialize(ve::engine::Window& window)
	{
		VulkanBackendSettings settings{};
		return Initialize(settings, window);
	}

	/** Initializes Vulkan instance, surface, physical device and logical device. */
	bool VulkanBackend::Initialize(const VulkanBackendSettings& settings, ve::engine::Window& window)
	{
		VulkanContextSettings context_settings = settings.context;
		context_settings.required_extensions = window.RequiredVulkanInstanceExtensions();
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Initializing Vulkan backend");
		if (!context_.Initialize(context_settings)) return false;
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Creating Vulkan window surface");
		if (!surface_.Create(context_.Instance(), window)) { VE_LOG_CATEGORY_ERROR(ve::log::category::Render, "Vulkan surface creation failed"); Release(); return false; }
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Selecting Vulkan physical device");
		if (!physical_device_.Select(context_.Instance(), surface_.Handle())) { VE_LOG_CATEGORY_ERROR(ve::log::category::Render, "No suitable Vulkan physical device found"); Release(); return false; }
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Creating Vulkan logical device");
		if (!device_.Create(physical_device_.Handle(), physical_device_.QueueFamilies())) { VE_LOG_CATEGORY_ERROR(ve::log::category::Render, "Vulkan logical device creation failed"); Release(); return false; }
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Creating Vulkan memory allocator");
		if (!allocator_.Initialize(context_.Instance(), physical_device_.Handle(), device_.Handle())) { VE_LOG_CATEGORY_ERROR(ve::log::category::Render, "Vulkan memory allocator creation failed"); Release(); return false; }
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Creating Vulkan swapchain");
		if (!swapchain_.Create(physical_device_.Handle(), device_.Handle(), surface_.Handle(), window.GetWidth(), window.GetHeight())) { VE_LOG_CATEGORY_ERROR(ve::log::category::Render, "Vulkan swapchain creation failed"); Release(); return false; }
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan backend initialized");
		return true;
	}

	/** Releases Vulkan startup state. */
	void VulkanBackend::Release()
	{
		swapchain_.Release();
		allocator_.Release();
		device_.Release();
		surface_.Release();
		context_.Release();
	}

	/** Returns capabilities expected from the Vulkan renderer path. */
	RenderBackendCapabilities VulkanBackend::Capabilities() const noexcept
	{
		return { true, true, false, context_.IsInitialized() };
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

	/** Reports whether the Vulkan context is initialized. */
	bool VulkanBackend::IsInitialized() const noexcept { return context_.IsInitialized(); }
}
