#pragma once

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
}
