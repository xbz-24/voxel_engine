#pragma once

#include "RuntimeSettings.h"

#include <optional>

namespace ve::engine
{
	struct MouseLookState
	{
		double previous_cursor_x = 0.0;
		double previous_cursor_y = 0.0;
		bool is_first_input_event = true;
	};

	struct MouseLookDelta
	{
		float yaw_degrees = 0.0f;
		float pitch_degrees = 0.0f;
	};

	void ResetMouseLookState(MouseLookState& mouse_look_state) noexcept;

	[[nodiscard]] std::optional<MouseLookDelta> ConsumeMouseLookDelta(
		MouseLookState& mouse_look_state,
		double current_cursor_x,
		double current_cursor_y,
		const ve::gameplay::MouseLookSettings& settings) noexcept;
}
