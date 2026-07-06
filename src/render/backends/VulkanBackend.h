#pragma once

#include "RenderBackend.h"
#include "VulkanBackendSettings.h"
#include "VulkanContext.h"
#include "VulkanDebugLabels.h"
#include "VulkanDevice.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"

#include <string>
#include <utility>

namespace ve::engine { class Window; }

namespace ve::rendering
{
	enum class VulkanBackendInitializationFailure
	{
		None,
		ContextCreationFailed,
		SurfaceCreationFailed,
		PhysicalDeviceSelectionFailed,
		LogicalDeviceCreationFailed,
		AllocatorCreationFailed,
		SwapchainCreationFailed
	};

	struct VulkanBackendInitializationResult
	{
		VulkanBackendInitializationFailure failure = VulkanBackendInitializationFailure::None;
		std::string message;

		[[nodiscard]] static VulkanBackendInitializationResult Success()
		{
			return {};
		}

		[[nodiscard]] static VulkanBackendInitializationResult Failure(
			VulkanBackendInitializationFailure failure,
			std::string message)
		{
			return VulkanBackendInitializationResult{ failure, std::move(message) };
		}

		[[nodiscard]] explicit operator bool() const noexcept
		{
			return failure == VulkanBackendInitializationFailure::None;
		}
	};

	/** High-level object that owns Vulkan startup state for the renderer. */
	class VulkanBackend final : public RenderBackend
	{
	public:
		/** Initializes with default Vulkan backend settings. */
		[[nodiscard]] bool Initialize(ve::engine::Window& window);

		/** @param settings Vulkan backend settings. @param window Presentation window. @return True when full backend startup succeeds. */
		[[nodiscard]] bool Initialize(const VulkanBackendSettings& settings, ve::engine::Window& window);

		/** @param settings Vulkan backend settings. @param window Presentation window. @return Startup stage and message. */
		[[nodiscard]] VulkanBackendInitializationResult InitializeDetailed(
			const VulkanBackendSettings& settings,
			ve::engine::Window& window);

		/** Releases all Vulkan backend state. */
		void Release();

		/** @return Render backend capabilities exposed to high-level systems. */
		[[nodiscard]] RenderBackendCapabilities Capabilities() const noexcept override;

		/** @return API represented by this backend. */
		[[nodiscard]] GraphicsApi Api() const noexcept override;

		/** @return Vulkan context owned by the backend. */
		[[nodiscard]] VulkanContext& Context() noexcept;

		/** @return Vulkan surface owned by the backend. */
		[[nodiscard]] VulkanSurface& Surface() noexcept;

		/** @return Selected physical device. */
		[[nodiscard]] VulkanPhysicalDevice& PhysicalDevice() noexcept;

		/** @return Logical Vulkan device. */
		[[nodiscard]] VulkanDevice& Device() noexcept;

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
