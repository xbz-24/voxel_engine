#pragma once

#include "VulkanMinecraftDemoSettings.h"

#include <volk.h>

namespace ve::engine { class Window; }

namespace ve::rendering
{
	class VulkanBackend;

	class VulkanImGuiOverlay
	{
	public:
		[[nodiscard]] bool Initialize(VulkanBackend& backend, ve::engine::Window& window, VkRenderPass render_pass);
		void BeginFrame(VulkanMinecraftDemoSettings& settings, const VulkanMinecraftDemoStats& stats);
		void Record(VkCommandBuffer command_buffer);
		void Release();

		[[nodiscard]] bool IsInitialized() const noexcept;
		[[nodiscard]] bool WantsMouseInput() const noexcept;

	private:
		VkDevice device_ = VK_NULL_HANDLE;
		bool initialized_ = false;
		bool has_draw_data_ = false;
	};
}
