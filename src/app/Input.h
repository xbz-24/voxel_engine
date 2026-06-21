#pragma once

#include <array>
#include <cstddef>

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
		Digit9,
		Count
	};

	/**
	 * Names the mouse buttons understood by the engine input layer.
	 */
	enum class MouseButton
	{
		Left,
		Right,
		Count
	};

	struct InputSnapshot
	{
		std::array<bool, static_cast<std::size_t>(Key::Count)> keys{};
		std::array<bool, static_cast<std::size_t>(MouseButton::Count)> mouse_buttons{};
	};

	/**
	 * Captures all engine input bindings from the native window for this frame.
	 *
	 * @param window Native window that owns the input state.
	 * @return Backend-neutral input snapshot.
	 */
	InputSnapshot CaptureInputSnapshot(GLFWwindow* window) noexcept;

	/**
	 * Reads whether a key is held down this frame.
	 *
	 * @param window Native window that owns the input state.
	 * @param key Engine key to query.
	 * @return True when the key is currently pressed.
	 */
	bool IsPressed(GLFWwindow* window, Key key) noexcept;

	/**
	 * Reads whether a key is held in a captured input snapshot.
	 *
	 * @param snapshot Captured input state.
	 * @param key Engine key to query.
	 * @return True when the key is currently pressed.
	 */
	bool IsPressed(const InputSnapshot& snapshot, Key key) noexcept;

	/**
	 * Reads whether a mouse button is held down this frame.
	 *
	 * @param window Native window that owns the input state.
	 * @param button Engine mouse button to query.
	 * @return True when the button is currently pressed.
	 */
	bool IsPressed(GLFWwindow* window, MouseButton button) noexcept;

	/**
	 * Reads whether a mouse button is held in a captured input snapshot.
	 *
	 * @param snapshot Captured input state.
	 * @param button Engine mouse button to query.
	 * @return True when the button is currently pressed.
	 */
	bool IsPressed(const InputSnapshot& snapshot, MouseButton button) noexcept;

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
	 * Converts a snapshot key state into a one-shot press event.
	 *
	 * @param snapshot Captured input state.
	 * @param key Engine key to query.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	bool WasPressed(const InputSnapshot& snapshot, Key key, bool& wasPressed) noexcept;

	/**
	 * Converts a held mouse button into a one-shot press event.
	 *
	 * @param window Native window that owns the input state.
	 * @param button Engine mouse button to query.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	bool WasPressed(GLFWwindow* window, MouseButton button, bool& wasPressed) noexcept;

	/**
	 * Converts a snapshot mouse state into a one-shot press event.
	 *
	 * @param snapshot Captured input state.
	 * @param button Engine mouse button to query.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	bool WasPressed(const InputSnapshot& snapshot, MouseButton button, bool& wasPressed) noexcept;
}
