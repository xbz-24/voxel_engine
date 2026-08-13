#include "VulkanBackend.h"

#include "Logger.h"
#include "VulkanBackendStartup.h"

namespace ve::rendering
{
	bool VulkanBackend::Initialize(ve::engine::Window& window)
	{
		VulkanBackendSettings settings{};
		return InitializeDetailed(settings, window).failure == VulkanBackendInitializationFailure::None;
	}

	bool VulkanBackend::Initialize(const VulkanBackendSettings& settings, ve::engine::Window& window)
	{
		return InitializeDetailed(settings, window).failure == VulkanBackendInitializationFailure::None;
	}

	VulkanBackendInitializationResult VulkanBackend::InitializeDetailed(
		const VulkanBackendSettings& settings,
		ve::engine::Window& window)
	{
		const VulkanBackendWindowSettings window_settings = CaptureVulkanWindowSettings(window);
		const VulkanContextSettings context_settings = BuildVulkanContextSettings(settings, window_settings);
		VulkanBackendInitializationResult result = VulkanBackendInitializationResult::Success();

		auto initialize_step = [&](const char* info_message,
			VulkanBackendInitializationFailure failure,
			const char* error_message,
			auto&& action) -> bool
		{
			VE_LOG_CATEGORY_INFO(ve::log::category::Render, info_message);
			if (!action())
			{
				VE_LOG_CATEGORY_ERROR(ve::log::category::Render, error_message);
				result = VulkanBackendInitializationResult::Failure(failure, error_message);
				return false;
			}
			return true;
		};

		const VulkanSwapchainSettings swapchain_settings = BuildVulkanSwapchainSettings(settings, window_settings);
		const bool is_initialized =
			initialize_step("Initializing Vulkan context",
				VulkanBackendInitializationFailure::ContextCreationFailed,
				"Context creation failed",
				[&] { return context_.Initialize(context_settings); }) &&
			initialize_step("Creating Vulkan window surface",
				VulkanBackendInitializationFailure::SurfaceCreationFailed,
				"Surface creation failed",
				[&] { return surface_.Create(context_.Instance(), window); }) &&
			initialize_step("Selecting Vulkan physical device",
				VulkanBackendInitializationFailure::PhysicalDeviceSelectionFailed,
				"No suitable physical device",
				[&] { return physical_device_.Select(context_.Instance(), BuildVulkanPhysicalDeviceCriteria(settings, surface_.Handle())); }) &&
			initialize_step("Creating Vulkan logical device",
				VulkanBackendInitializationFailure::LogicalDeviceCreationFailed,
				"Logical device creation failed",
				[&] { return device_.Create(physical_device_.Handle(), physical_device_.QueueFamilies(), settings.device); }) &&
			initialize_step("Creating Vulkan memory allocator",
				VulkanBackendInitializationFailure::AllocatorCreationFailed,
				"Allocator creation failed",
				[&] { return allocator_.Initialize(context_.Instance(), physical_device_.Handle(), device_.Handle()); }) &&
			initialize_step("Creating Vulkan swapchain",
				VulkanBackendInitializationFailure::SwapchainCreationFailed,
				"Swapchain creation failed",
				[&] { return swapchain_.Create(physical_device_.Handle(), device_.Handle(), surface_.Handle(), swapchain_settings); });

		if (!is_initialized)
		{
			Release();
			return result;
		}

		debug_labels_.Initialize(device_.Handle(), settings.context.enable_debug_utils);
		(void)debug_labels_.NameObject(
			VK_OBJECT_TYPE_DEVICE,
			VulkanDispatchableObjectHandle(device_.Handle()),
			"voxel_engine.logical_device");
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan backend initialized successfully");
		return VulkanBackendInitializationResult::Success();
	}

}
