#include "Input.h"

#include "InputMappings.h"
#include "Window.h"

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

	bool IsKeyPressed(GLFWwindow* window, ve::input::Key key) noexcept
	{
		return window && glfwGetKey(window, ve::input::detail::ToGlfwKey(key)) == GLFW_PRESS;
	}

	bool IsMouseButtonPressed(GLFWwindow* window, ve::input::MouseButton button) noexcept
	{
		return window &&
			glfwGetMouseButton(window, ve::input::detail::ToGlfwMouseButton(button)) == GLFW_PRESS;
	}
}

namespace ve::input
{
	InputSnapshot CaptureInputSnapshot(const ve::engine::Window& window) noexcept
	{
		GLFWwindow* native_window = window.GetNativeWindow();
		InputSnapshot snapshot;
		for (std::size_t index = 0; index < snapshot.keys.size(); ++index)
		{
			const Key key = static_cast<Key>(index);
			snapshot.keys[index] = IsKeyPressed(native_window, key);
		}
		for (std::size_t index = 0; index < snapshot.mouse_buttons.size(); ++index)
		{
			const MouseButton button = static_cast<MouseButton>(index);
			snapshot.mouse_buttons[index] = IsMouseButtonPressed(native_window, button);
		}
		return snapshot;
	}

	/// Reads whether a key is held down this frame.
	bool IsPressed(const ve::engine::Window& window, Key key) noexcept
	{
		return IsKeyPressed(window.GetNativeWindow(), key);
	}

	bool IsPressed(const InputSnapshot& snapshot, Key key) noexcept
	{
		const std::size_t index = static_cast<std::size_t>(key);
		return index < snapshot.keys.size() && snapshot.keys[index];
	}

	/// Reads whether a mouse button is held down this frame.
	bool IsPressed(const ve::engine::Window& window, MouseButton button) noexcept
	{
		return IsMouseButtonPressed(window.GetNativeWindow(), button);
	}

	bool IsPressed(const InputSnapshot& snapshot, MouseButton button) noexcept
	{
		const std::size_t index = static_cast<std::size_t>(button);
		return index < snapshot.mouse_buttons.size() && snapshot.mouse_buttons[index];
	}

	/// Converts a held key into a one-shot press event.
	bool WasPressed(const ve::engine::Window& window, Key key, bool& wasPressed) noexcept
	{
		return ConsumeRisingEdge(IsPressed(window, key), wasPressed);
	}

	bool WasPressed(const InputSnapshot& snapshot, Key key, bool& wasPressed) noexcept
	{
		return ConsumeRisingEdge(IsPressed(snapshot, key), wasPressed);
	}

	/// Converts a held mouse button into a one-shot press event.
	bool WasPressed(const ve::engine::Window& window, MouseButton button, bool& wasPressed) noexcept
	{
		return ConsumeRisingEdge(IsPressed(window, button), wasPressed);
	}

	bool WasPressed(const InputSnapshot& snapshot, MouseButton button, bool& wasPressed) noexcept
	{
		return ConsumeRisingEdge(IsPressed(snapshot, button), wasPressed);
	}
}
