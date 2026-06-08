#pragma once

struct GLFWwindow;

namespace ve::input
{
	/**
	 * Names the keyboard keys understood by the engine input layer.
	 */
	enum class Key
	{
		Escape,
		Up,
		Down,
		Left,
		Right,
		Enter,
		Space,
		LeftShift,
		W,
		A,
		S,
		D,
		F,
		F3,
		LeftBracket,
		RightBracket,
		Digit1,
		Digit2,
		Digit3,
		Digit4,
		Digit5,
		Digit6,
		Digit7,
		Digit8,
		Digit9
	};

	/**
	 * Names the mouse buttons understood by the engine input layer.
	 */
	enum class MouseButton
	{
		Left,
		Right
	};

	/**
	 * Reads whether a key is held down this frame.
	 *
	 * @param window Native window that owns the input state.
	 * @param key Engine key to query.
	 * @return True when the key is currently pressed.
	 */
	bool IsPressed(GLFWwindow* window, Key key) noexcept;

	/**
	 * Reads whether a mouse button is held down this frame.
	 *
	 * @param window Native window that owns the input state.
	 * @param button Engine mouse button to query.
	 * @return True when the button is currently pressed.
	 */
	bool IsPressed(GLFWwindow* window, MouseButton button) noexcept;

	/**
	 * Converts a held key into a one-shot press event.
	 *
	 * @param window Native window that owns the input state.
	 * @param key Engine key to query.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	bool WasPressed(GLFWwindow* window, Key key, bool& wasPressed) noexcept;

	/**
	 * Converts a held mouse button into a one-shot press event.
	 *
	 * @param window Native window that owns the input state.
	 * @param button Engine mouse button to query.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	bool WasPressed(GLFWwindow* window, MouseButton button, bool& wasPressed) noexcept;
}
