#include "MouseLookInput.h"

#include <algorithm>

namespace ve::engine
{
	void ResetMouseLookState(MouseLookState& mouse_look_state) noexcept
	{
		mouse_look_state.is_first_input_event = true;
	}

	std::optional<MouseLookDelta> ConsumeMouseLookDelta(
		MouseLookState& mouse_look_state,
		double current_cursor_x,
		double current_cursor_y,
		const ve::gameplay::MouseLookSettings& settings) noexcept
	{
		if (mouse_look_state.is_first_input_event)
		{
			mouse_look_state.previous_cursor_x = current_cursor_x;
			mouse_look_state.previous_cursor_y = current_cursor_y;
			mouse_look_state.is_first_input_event = false;
			return std::nullopt;
		}

		const double raw_delta_x = current_cursor_x - mouse_look_state.previous_cursor_x;
		const double raw_delta_y = mouse_look_state.previous_cursor_y - current_cursor_y;
		mouse_look_state.previous_cursor_x = current_cursor_x;
		mouse_look_state.previous_cursor_y = current_cursor_y;

		const float sensitivity_degrees_per_pixel =
			std::max(0.0f, settings.sensitivity_degrees_per_pixel);
		const float yaw_axis = settings.invert_x_axis ? -1.0f : 1.0f;
		const float pitch_axis = settings.invert_y_axis ? -1.0f : 1.0f;

		return MouseLookDelta{
			static_cast<float>(raw_delta_x) * sensitivity_degrees_per_pixel * yaw_axis,
			static_cast<float>(raw_delta_y) * sensitivity_degrees_per_pixel * pitch_axis
		};
	}
}
