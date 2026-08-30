#include "VulkanRuntimePresentationState.h"

namespace ve::engine
{
	namespace
	{
		[[nodiscard]] bool SameSize(WindowSize left, WindowSize right) noexcept
		{
			return left.width == right.width && left.height == right.height;
		}
	}

	VulkanPresentationAction VulkanRuntimePresentationState::NextAction(
		WindowSize framebuffer_size,
		bool vsync_enabled) const noexcept
	{
		if (framebuffer_size.width <= 0 || framebuffer_size.height <= 0)
		{
			return VulkanPresentationAction::Defer;
		}
		if (refresh_required_ ||
			!SameSize(framebuffer_size, observed_framebuffer_size_) ||
			vsync_enabled != observed_vsync_enabled_)
		{
			return VulkanPresentationAction::Rebuild;
		}
		return VulkanPresentationAction::Draw;
	}

	void VulkanRuntimePresentationState::Commit(
		WindowSize framebuffer_size,
		bool vsync_enabled) noexcept
	{
		observed_framebuffer_size_ = framebuffer_size;
		observed_vsync_enabled_ = vsync_enabled;
		refresh_required_ = false;
	}

	void VulkanRuntimePresentationState::RequireRefresh() noexcept
	{
		refresh_required_ = true;
	}
}
