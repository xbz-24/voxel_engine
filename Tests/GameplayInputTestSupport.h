#pragma once

#include "Input.h"

inline void PressKey(ve::input::InputSnapshot& input, ve::input::Key key) noexcept
{
	input.keys[ve::input::ToKeyIndex(key)] = true;
}

inline void ReleaseKey(ve::input::InputSnapshot& input, ve::input::Key key) noexcept
{
	input.keys[ve::input::ToKeyIndex(key)] = false;
}

inline void PressMouseButton(
	ve::input::InputSnapshot& input,
	ve::input::MouseButton button) noexcept
{
	input.mouse_buttons[ve::input::ToMouseButtonIndex(button)] = true;
}
