#include "VulkanBackend.h"

#include "Logger.h"
#include "Window.h"

#include <algorithm>
#include <cstring>
#include <span>
#include <vector>

namespace ve::rendering
{
	namespace
	{
		struct VulkanBackendWindowSettings
		{
			std::vector<const char*> required_instance_extensions{};
			int swapchain_width = 1;
			int swapchain_height = 1;
			bool enable_vsync = false;
		};

		void AppendUniqueExtensions(
			std::vector<const char*>& target_extensions,
			std::span<const char* const> source_extensions)
		{
			for (const char* extension_name : source_extensions)
			{
				const auto existing = std::ranges::find_if(target_extensions, [extension_name](const char* target_extension)
				{
					return std::strcmp(target_extension, extension_name) == 0;
				});
				if (existing == target_extensions.end()) target_extensions.push_back(extension_name);
			}
		}

		[[nodiscard]] VulkanBackendWindowSettings CaptureWindowSettings(const ve::engine::Window& window)
		{
			return VulkanBackendWindowSettings{
				window.RequiredVulkanInstanceExtensions(),
				std::max(window.GetWidth(), 1),
				std::max(window.GetHeight(), 1),
				window.IsVSyncEnabled()
			};
		}

		[[nodiscard]] VulkanContextSettings BuildContextSettings(
			const VulkanBackendSettings& settings,
			const VulkanBackendWindowSettings& window_settings)
		{
			VulkanContextSettings context_settings = settings.context;
			AppendUniqueExtensions(context_settings.required_extensions, window_settings.required_instance_extensions);
			return context_settings;
		}

		[[nodiscard]] VulkanPhysicalDeviceSelectionCriteria BuildPhysicalDeviceCriteria(
			const VulkanBackendSettings& settings,
			VkSurfaceKHR surface)
		{
			return VulkanPhysicalDeviceSelectionCriteria{
				settings.physical_device,
				surface,
				settings.device.required_extensions
			};
		}

		[[nodiscard]] VulkanSwapchainSettings BuildSwapchainSettings(
			const VulkanBackendSettings& settings,
			const VulkanBackendWindowSettings& window_settings)
		{
			VulkanSwapchainSettings swapchain_settings = settings.swapchain;
			if (swapchain_settings.width <= 0) swapchain_settings.width = window_settings.swapchain_width;
			if (swapchain_settings.height <= 0) swapchain_settings.height = window_settings.swapchain_height;
			swapchain_settings.enable_vsync = window_settings.enable_vsync;
			return swapchain_settings;
		}
	}

	bool VulkanBackend::Initialize(ve::engine::Window& window)
	{
		VulkanBackendSettings settings{};
		return Initialize(settings, window);
	}

	bool VulkanBackend::Initialize(const VulkanBackendSettings& settings, ve::engine::Window& window)
	{
		const VulkanBackendWindowSettings window_settings = CaptureWindowSettings(window);
		const VulkanContextSettings context_settings = BuildContextSettings(settings, window_settings);

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

		const VulkanSwapchainSettings swapchain_settings = BuildSwapchainSettings(settings, window_settings);
		const bool is_initialized =
			initialize_step("Initializing Vulkan context", "Context creation failed", [&] { return context_.Initialize(context_settings); }) &&
			initialize_step("Creating Vulkan window surface", "Surface creation failed", [&] { return surface_.Create(context_.Instance(), window); }) &&
			initialize_step("Selecting Vulkan physical device", "No suitable physical device", [&] { return physical_device_.Select(context_.Instance(), BuildPhysicalDeviceCriteria(settings, surface_.Handle())); }) &&
			initialize_step("Creating Vulkan logical device", "Logical device creation failed", [&] { return device_.Create(physical_device_.Handle(), physical_device_.QueueFamilies(), settings.device); }) &&
			initialize_step("Creating Vulkan memory allocator", "Allocator creation failed", [&] { return allocator_.Initialize(context_.Instance(), physical_device_.Handle(), device_.Handle()); }) &&
			initialize_step("Creating Vulkan swapchain", "Swapchain creation failed", [&] { return swapchain_.Create(physical_device_.Handle(), device_.Handle(), surface_.Handle(), swapchain_settings); });

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
