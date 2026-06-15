#pragma once

#include "Input.h"

namespace ve::input::detail
{
	/**
	 * Converts an engine key to the backend key code.
	 *
	 * @param key Engine key to convert.
	 * @return GLFW key code used by the backend.
	 */
	int ToGlfwKey(Key key) noexcept;

	/**
	 * Converts an engine mouse button to the backend button code.
	 *
	 * @param button Engine mouse button to convert.
	 * @return GLFW mouse button code used by the backend.
	 */
	int ToGlfwMouseButton(MouseButton button) noexcept;
}
