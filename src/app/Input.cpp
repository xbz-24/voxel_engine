#include "Input.h"

#include "InputMappings.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace
{
	/// Updates a cached input state and reports a rising edge.
	bool ConsumeRisingEdge(bool isPressed, bool& wasPressed) noexcept
	{
		const bool wasJustPressed = isPressed && !wasPressed;
		wasPressed = isPressed;
		return wasJustPressed;
	}
}

namespace ve::input
{
	/// Reads whether a key is held down this frame.
	bool IsPressed(GLFWwindow* window, Key key) noexcept
	{
		return window && glfwGetKey(window, detail::ToGlfwKey(key)) == GLFW_PRESS;
	}

	/// Reads whether a mouse button is held down this frame.
	bool IsPressed(GLFWwindow* window, MouseButton button) noexcept
	{
		return window && glfwGetMouseButton(window, detail::ToGlfwMouseButton(button)) == GLFW_PRESS;
	}

	/// Converts a held key into a one-shot press event.
	bool WasPressed(GLFWwindow* window, Key key, bool& wasPressed) noexcept
	{
		return ConsumeRisingEdge(IsPressed(window, key), wasPressed);
	}

	/// Converts a held mouse button into a one-shot press event.
	bool WasPressed(GLFWwindow* window, MouseButton button, bool& wasPressed) noexcept
	{
		return ConsumeRisingEdge(IsPressed(window, button), wasPressed);
	}
}
