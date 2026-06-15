#include "InputMappings.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ve::input::detail
{
	/// Converts an engine key to the backend key code.
	int ToGlfwKey(Key key) noexcept
	{
		switch (key)
		{
		case Key::Escape: return GLFW_KEY_ESCAPE;
		case Key::Up: return GLFW_KEY_UP;
		case Key::Down: return GLFW_KEY_DOWN;
		case Key::Left: return GLFW_KEY_LEFT;
		case Key::Right: return GLFW_KEY_RIGHT;
		case Key::Enter: return GLFW_KEY_ENTER;
		case Key::Space: return GLFW_KEY_SPACE;
		case Key::LeftShift: return GLFW_KEY_LEFT_SHIFT;
		case Key::W: return GLFW_KEY_W;
		case Key::A: return GLFW_KEY_A;
		case Key::S: return GLFW_KEY_S;
		case Key::D: return GLFW_KEY_D;
		case Key::F: return GLFW_KEY_F;
		case Key::F3: return GLFW_KEY_F3;
		case Key::LeftBracket: return GLFW_KEY_LEFT_BRACKET;
		case Key::RightBracket: return GLFW_KEY_RIGHT_BRACKET;
		case Key::Digit1: return GLFW_KEY_1;
		case Key::Digit2: return GLFW_KEY_2;
		case Key::Digit3: return GLFW_KEY_3;
		case Key::Digit4: return GLFW_KEY_4;
		case Key::Digit5: return GLFW_KEY_5;
		case Key::Digit6: return GLFW_KEY_6;
		case Key::Digit7: return GLFW_KEY_7;
		case Key::Digit8: return GLFW_KEY_8;
		case Key::Digit9: return GLFW_KEY_9;
		}
		return GLFW_KEY_UNKNOWN;
	}

	/// Converts an engine mouse button to the backend button code.
	int ToGlfwMouseButton(MouseButton button) noexcept
	{
		switch (button)
		{
		case MouseButton::Left: return GLFW_MOUSE_BUTTON_LEFT;
		case MouseButton::Right: return GLFW_MOUSE_BUTTON_RIGHT;
		}
		return GLFW_MOUSE_BUTTON_LEFT;
	}
}
