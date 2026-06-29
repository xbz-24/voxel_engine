#pragma once

#include <array>
#include <cstddef>

struct GLFWwindow;

namespace ve::input
{
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

	InputSnapshot CaptureInputSnapshot(GLFWwindow* window) noexcept;
	bool IsPressed(GLFWwindow* window, Key key) noexcept;
	bool IsPressed(const InputSnapshot& snapshot, Key key) noexcept;
	bool IsPressed(GLFWwindow* window, MouseButton button) noexcept;
	bool IsPressed(const InputSnapshot& snapshot, MouseButton button) noexcept;
	bool WasPressed(GLFWwindow* window, Key key, bool& wasPressed) noexcept;
	bool WasPressed(const InputSnapshot& snapshot, Key key, bool& wasPressed) noexcept;
	bool WasPressed(GLFWwindow* window, MouseButton button, bool& wasPressed) noexcept;
	bool WasPressed(const InputSnapshot& snapshot, MouseButton button, bool& wasPressed) noexcept;
}
