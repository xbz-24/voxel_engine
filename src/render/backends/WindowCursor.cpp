#include "Window.h"

namespace
{
	/// Converts an engine cursor mode to the GLFW cursor mode.
	int ToGlfwCursorMode(ve::engine::Window::CursorMode mode) noexcept
	{
		switch (mode)
		{
		case ve::engine::Window::CursorMode::Normal: return GLFW_CURSOR_NORMAL;
		case ve::engine::Window::CursorMode::Captured: return GLFW_CURSOR_DISABLED;
		}
		return GLFW_CURSOR_NORMAL;
	}
}

/// Applies the cursor mode used by menus and first-person camera control.
void ve::engine::Window::SetCursorMode(CursorMode mode)
{
	glfwSetInputMode(_window, GLFW_CURSOR, ToGlfwCursorMode(mode));
}
