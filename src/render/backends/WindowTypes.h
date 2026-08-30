#pragma once

namespace ve::engine
{
	class Window;

	enum class WindowCursorMode
	{
		Normal,
		Captured
	};

	struct WindowSize
	{
		int width = 1;
		int height = 1;
	};

	struct WindowCursorPosition
	{
		double x = 0.0;
		double y = 0.0;
	};

	struct NativeWindowHandle
	{
		void* opaque_handle = nullptr;
	};

	struct WindowCallbackContext
	{
		Window* window = nullptr;
		void* userData = nullptr;
	};
}
