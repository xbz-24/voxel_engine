#pragma once

#include "RenderBackend.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"

namespace ve::engine { class Window; }

namespace ve::rendering
{
	struct VulkanBackendSettings
	{
		VulkanContextSettings context{};
	};

	/** High-level object that owns Vulkan startup state for the renderer. */
	class VulkanBackend final : public RenderBackend
	{
	public:
		/** Initializes with default Vulkan backend settings. */
		[[nodiscard]] bool Initialize(ve::engine::Window& window);

		/** @param settings Vulkan backend settings. @param window Presentation window. @return True when full backend startup succeeds. */
		[[nodiscard]] bool Initialize(const VulkanBackendSettings& settings, ve::engine::Window& window);

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

		/** @return True when the backend owns a Vulkan instance. */
		[[nodiscard]] bool IsInitialized() const noexcept;

	private:
		VulkanContext context_;
		VulkanSurface surface_;
		VulkanPhysicalDevice physical_device_;
		VulkanDevice device_;
		VulkanMemoryAllocator allocator_;
		VulkanSwapchain swapchain_;
	};
}
