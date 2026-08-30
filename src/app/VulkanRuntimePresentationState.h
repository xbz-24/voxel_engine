#pragma once

#include "WindowTypes.h"

namespace ve::engine
{
	enum class VulkanPresentationAction
	{
		Draw,
		Defer,
		Rebuild
	};

	class VulkanRuntimePresentationState
	{
	public:
		[[nodiscard]] VulkanPresentationAction NextAction(
			WindowSize framebuffer_size,
			bool vsync_enabled) const noexcept;
		void Commit(WindowSize framebuffer_size, bool vsync_enabled) noexcept;
		void RequireRefresh() noexcept;

	private:
		WindowSize observed_framebuffer_size_{ 0, 0 };
		bool observed_vsync_enabled_ = false;
		bool refresh_required_ = false;
	};
}
