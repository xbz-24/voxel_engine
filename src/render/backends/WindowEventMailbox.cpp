#include "WindowEvents.h"

namespace ve::engine
{
	void WindowEventMailbox::RecordFramebufferResize(int width, int height) noexcept
	{
		pending_framebuffer_resize_ = WindowFramebufferResizeEvent{ width, height };
		has_pending_framebuffer_resize_ = true;
	}

	std::vector<WindowEvent> WindowEventMailbox::Drain()
	{
		if (!has_pending_framebuffer_resize_) return {};
		std::vector<WindowEvent> events{
			WindowEvent{
				.kind = WindowEvent::Kind::FramebufferResized,
				.framebuffer_resized = pending_framebuffer_resize_
			}
		};
		has_pending_framebuffer_resize_ = false;
		return events;
	}
}
