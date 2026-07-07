#include "RuntimeInputRouter.h"

#include "Camera.h"
#include "Window.h"

namespace ve::engine
{
	void RuntimeInputRouter::BindMouseLook(
		Camera& camera,
		const bool& ui_captures_mouse,
		const ve::gameplay::MouseLookSettings& mouse_look_settings) noexcept
	{
		camera_ = &camera;
		ui_captures_mouse_ = &ui_captures_mouse;
		mouse_look_settings_ = &mouse_look_settings;
		ResetMouseLookState(mouse_look_state_);
	}

	void RuntimeInputRouter::ClearMouseLookBinding() noexcept
	{
		camera_ = nullptr;
		ui_captures_mouse_ = nullptr;
		mouse_look_settings_ = nullptr;
		ResetMouseLookState(mouse_look_state_);
	}

	std::optional<MouseLookDelta> RuntimeInputRouter::HandleCursorPosition(
		double current_cursor_x,
		double current_cursor_y) noexcept
	{
		if (camera_ == nullptr)
		{
			return std::nullopt;
		}
		if (ui_captures_mouse_ != nullptr && *ui_captures_mouse_)
		{
			ResetMouseLookState(mouse_look_state_);
			return std::nullopt;
		}

		const ve::gameplay::MouseLookSettings mouse_look_settings =
			mouse_look_settings_ != nullptr ? *mouse_look_settings_ : ve::gameplay::MouseLookSettings{};
		std::optional<MouseLookDelta> mouse_look_delta = ConsumeMouseLookDelta(
			mouse_look_state_,
			current_cursor_x,
			current_cursor_y,
			mouse_look_settings);
		if (!mouse_look_delta.has_value())
		{
			return std::nullopt;
		}

		camera_->Yaw(mouse_look_delta->yaw_degrees);
		camera_->Pitch(mouse_look_delta->pitch_degrees);
		return mouse_look_delta;
	}

	void RuntimeInputRouter::CursorPositionCallback(GLFWwindow* window, double current_cursor_x, double current_cursor_y) noexcept
	{
		RuntimeInputRouter* input_router =
			static_cast<RuntimeInputRouter*>(Window::GetCallbackUserData(window));
		if (input_router != nullptr)
		{
			(void)input_router->HandleCursorPosition(current_cursor_x, current_cursor_y);
		}
	}
}
