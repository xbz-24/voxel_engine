#pragma once

#include "CoreTypes.h"

#include <array>
#include <cstddef>

namespace ve::engine
{
	class Window;
}

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
		F1,
		F2,
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

	inline constexpr std::size_t KeyCount = ve::core::ToIndex(Key::Count);
	inline constexpr std::size_t MouseButtonCount = ve::core::ToIndex(MouseButton::Count);

	[[nodiscard]] constexpr std::size_t ToKeyIndex(Key key) noexcept
	{
		return ve::core::ToIndex(key);
	}

	[[nodiscard]] constexpr std::size_t ToMouseButtonIndex(MouseButton button) noexcept
	{
		return ve::core::ToIndex(button);
	}

	struct InputSnapshot
	{
		std::array<bool, KeyCount> keys{};
		std::array<bool, MouseButtonCount> mouse_buttons{};
	};

	InputSnapshot CaptureInputSnapshot(const ve::engine::Window& window) noexcept;
	bool IsPressed(const ve::engine::Window& window, Key key) noexcept;
	bool IsPressed(const InputSnapshot& snapshot, Key key) noexcept;
	bool IsPressed(const ve::engine::Window& window, MouseButton button) noexcept;
	bool IsPressed(const InputSnapshot& snapshot, MouseButton button) noexcept;
	bool WasPressed(const ve::engine::Window& window, Key key, bool& wasPressed) noexcept;
	bool WasPressed(const InputSnapshot& snapshot, Key key, bool& wasPressed) noexcept;
	bool WasPressed(const ve::engine::Window& window, MouseButton button, bool& wasPressed) noexcept;
	bool WasPressed(const InputSnapshot& snapshot, MouseButton button, bool& wasPressed) noexcept;
}
