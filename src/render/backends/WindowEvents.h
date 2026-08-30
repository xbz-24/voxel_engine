#pragma once

#include <vector>

namespace ve::engine
{
	struct WindowFramebufferResizeEvent
	{
		int width = 0;
		int height = 0;
	};

	struct WindowEvent
	{
		enum class Kind
		{
			FramebufferResized
		};

		Kind kind = Kind::FramebufferResized;
		WindowFramebufferResizeEvent framebuffer_resized{};
	};

	/** Coalesces native window callbacks without allocating inside the C callback. */
	class WindowEventMailbox
	{
	public:
		void RecordFramebufferResize(int width, int height) noexcept;
		[[nodiscard]] std::vector<WindowEvent> Drain();

	private:
		WindowFramebufferResizeEvent pending_framebuffer_resize_{};
		bool has_pending_framebuffer_resize_ = false;
	};
}
