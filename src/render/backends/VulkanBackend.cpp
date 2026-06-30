#include "VulkanBackend.h"

#include "Logger.h"
#include "Window.h"

namespace ve::rendering
{
	bool VulkanBackend::Initialize(ve::engine::Window& window)
	{
		VulkanBackendSettings settings{};
		return Initialize(settings, window);
	}

	bool VulkanBackend::Initialize(const VulkanBackendSettings& settings, ve::engine::Window& window)
	{
		VulkanContextSettings context_settings = settings.context;
		context_settings.required_extensions = window.RequiredVulkanInstanceExtensions();

		auto initialize_step = [&](const char* info_message, const char* error_message, auto&& action) -> bool
		{
			VE_LOG_CATEGORY_INFO(ve::log::category::Render, info_message);
			if (!action())
			{
				VE_LOG_CATEGORY_ERROR(ve::log::category::Render, error_message);
				return false;
			}
			return true;
		};

		const bool is_initialized =
			initialize_step("Initializing Vulkan context", "Context creation failed", [&] { return context_.Initialize(context_settings); }) &&
			initialize_step("Creating Vulkan window surface", "Surface creation failed", [&] { return surface_.Create(context_.Instance(), window); }) &&
			initialize_step("Selecting Vulkan physical device", "No suitable physical device", [&] { return physical_device_.Select(context_.Instance(), surface_.Handle()); }) &&
			initialize_step("Creating Vulkan logical device", "Logical device creation failed", [&] { return device_.Create(physical_device_.Handle(), physical_device_.QueueFamilies()); }) &&
			initialize_step("Creating Vulkan memory allocator", "Allocator creation failed", [&] { return allocator_.Initialize(context_.Instance(), physical_device_.Handle(), device_.Handle()); }) &&
			initialize_step("Creating Vulkan swapchain", "Swapchain creation failed", [&] { return swapchain_.Create(physical_device_.Handle(), device_.Handle(), surface_.Handle(), window.GetWidth(), window.GetHeight(), window.IsVSyncEnabled()); });

		if (!is_initialized)
		{
			Release();
			return false;
		}

		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan backend initialized successfully");
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
