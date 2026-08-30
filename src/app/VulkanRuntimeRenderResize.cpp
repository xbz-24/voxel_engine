#include "VulkanRuntimeRenderDriver.h"

#include "RenderViewFactory.h"
#include "Window.h"

#include <volk.h>

namespace ve::engine
{
	bool VulkanRuntimeRenderDriver::InitializeFrameOrchestrator(Window& window)
	{
		if (!backend_) return false;
		return orchestrator_.Initialize(*backend_, window,
			configuration_.block_texture_directory,
			configuration_.shader_directory,
			configuration_.render_style,
			configuration_.enable_imgui_overlay);
	}

	bool VulkanRuntimeRenderDriver::CreateRenderView()
	{
		if (!backend_) return false;
		view_ = RenderViewFactory::Create({ backend_->Api(), nullptr, backend_.get() });
		return view_ != nullptr;
	}

	bool VulkanRuntimeRenderDriver::RebuildPresentation(
		Window& window,
		WindowSize framebuffer_size)
	{
		if (!backend_ || framebuffer_size.width <= 0 || framebuffer_size.height <= 0)
		{
			return false;
		}
		if (vkDeviceWaitIdle(backend_->Device().Handle()) != VK_SUCCESS) return false;
		if (view_) view_->ReleaseCachedResources();
		view_.reset();
		orchestrator_.Release();
		if (!backend_->RecreateSwapchain(framebuffer_size.width,
			framebuffer_size.height,
			window.IsVSyncEnabled()))
		{
			return false;
		}
		if (!InitializeFrameOrchestrator(window) || !CreateRenderView()) return false;
		presentation_state_.Commit(framebuffer_size, window.IsVSyncEnabled());
		return true;
	}
}
