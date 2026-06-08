#include "Window.h"

namespace
{
	/// Converts an engine cursor mode to the GLFW cursor mode.
	int ToGlfwCursorMode(Window::CursorMode mode) noexcept
	{
		switch (mode)
		{
		case Window::CursorMode::Normal: return GLFW_CURSOR_NORMAL;
		case Window::CursorMode::Captured: return GLFW_CURSOR_DISABLED;
		}
		return GLFW_CURSOR_NORMAL;
	}
}

/// Applies the cursor mode used by menus and first-person camera control.
void Window::SetCursorMode(CursorMode mode)
{
	glfwSetInputMode(_window, GLFW_CURSOR, ToGlfwCursorMode(mode));
}
